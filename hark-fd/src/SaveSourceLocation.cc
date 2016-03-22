/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include <math.h>
#include "Source.h"

using namespace std;
//using namespace boost;
using namespace FD;

#if defined(ENABLE_DEBUGPRINT)

    #define DEBUG_PRINT(format, args...)                \
    fprintf(stderr, "[%s : %s, line %d] "format"\n",      \
        __FUNCTION__, __FILE__, __LINE__, ##args)

#else

    #define DEBUG_PRINT(format, args...)

#endif // #if defined(ENABLE_DEBUGPRINT)

class SaveSourceLocation;

DECLARE_NODE(SaveSourceLocation);

#if !defined(ENABLE_HARKIO3)


/*Node
 *
 * @name SaveSourceLocation
 * @category HARK:Localization
 * @description Save source locations as a text file.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description The same as input.
 *
 * @parameter_name FILENAME
 * @parameter_type string
 * @parameter_description The file name for saving source locations.
 *
 END*/

class SaveSourceLocation : public BufferedNode {
	int sourcesID;
	int outputID;

    string filename;

    ofstream fout;

public:
	SaveSourceLocation(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
    {
        sourcesID = addInput("SOURCES");
        outputID = addOutput("OUTPUT");

        filename = object_cast<string>(parameters.get("FILENAME"));
        if (filename.c_str() == '\0') {
            throw new NodeException(NULL, string("FILENAME is empty."), __FILE__, __LINE__);
        }

        fout.open(filename.c_str());
        if(fout.fail())
        {
            throw new NodeException(NULL, string("Can't open file'")+filename+"'.", __FILE__, __LINE__);
        }

		inOrder = true;
	}

    void calculate(int output_id, int count, Buffer &out)
    {
        RCPtr<Vector<ObjectRef> > pSources = getInput(sourcesID, count);
		const Vector<ObjectRef>& sources = *pSources;

        out[count] = pSources;

        fout << count << " ";
        for (int i = 0; i < sources.size(); i++) {
            RCPtr<Source> src = sources[i];

            fout << src->id << " "
                 << src->x[0] << " "
                 << src->x[1] << " "
                 << src->x[2] << " ";
        }
        fout << endl;
    }
};


#else // !defined(ENABLE_HARKIO3)

#include <string.h> 
#include "libharkio3.h"

/*Node
 *
 * @name SaveSourceLocation
 * @category HARK:Localization
 * @description Save source locations as a text file.
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description The same as input.
 *
 * @parameter_name FILENAME
 * @parameter_type string
 * @parameter_description The file name for saving source locations. 
 * 
 END*/

class SaveSourceLocation : public BufferedNode {
    int sourcesID;
    int outputID;
    string filename;
    FILE* fout;

public:
    SaveSourceLocation(string nodeName, ParameterSet params): 
        BufferedNode(nodeName, params),
        fout(NULL)
    {
        sourcesID = addInput("SOURCES");
        outputID = addOutput("OUTPUT");

        filename = object_cast<string>(parameters.get("FILENAME"));
        if (filename.c_str() == '\0') {
            throw new NodeException(NULL, string("FILENAME is empty."), __FILE__, __LINE__);
        }

        if (!(fout = fopen(filename.c_str(), "w"))
            || harkio_XML_writeHead(fout) != EXIT_SUCCESS) {
            throw new NodeException(NULL, string("Can't open file'")+filename+"'.", __FILE__, __LINE__);
        }

        inOrder = true;
    }

    ~SaveSourceLocation(void) 
    {
        if (harkio_XML_writeTail(fout) != EXIT_SUCCESS) {
            throw new NodeException(NULL, string("Can't finalizing writing xml.'"), __FILE__, __LINE__);
        }
        fclose(fout);
    }

    void calculate(int output_id, int count, Buffer &out)
    {
        //static const char* type = "tsp";

        RCPtr<Vector<ObjectRef> > pSources = getInput(sourcesID, count);
        const Vector<ObjectRef>& sources = *pSources;

        out[count] = pSources;

        harkio_Position* tmp[sources.size()];
        for (int i = 0; i < sources.size(); i++) {
            RCPtr<Source> src = sources[i];
            harkio_Position* pos = harkio_Position_new(
                src->id,
                Cartesian,
                src->x,
                NULL
            );
            tmp[i] = pos;
        }


        harkio_Positions* positions = harkio_Positions_new(
            NULL,
            count,
            sources.size(),
            tmp
        );

        if (harkio_XML_writePositions(positions, fout) != EXIT_SUCCESS) {
            throw new NodeException(NULL, string("Can't write positions data into xml file.'"), __FILE__, __LINE__);
        }

        return;
    }
};

#endif // !defined(ENABLE_HARKIO3)