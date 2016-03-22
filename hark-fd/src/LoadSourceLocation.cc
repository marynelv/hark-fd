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

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace FD;

class LoadSourceLocation;

#if defined(ENABLE_DEBUGPRINT)

    #define DEBUG_PRINT(format, args...)                \
    fprintf(stderr, "[%s : %s, line %d] "format"\n",      \
        __FUNCTION__, __FILE__, __LINE__, ##args)

#else

    #define DEBUG_PRINT(format, args...)

#endif // #if defined(ENABLE_DEBUGPRINT)


DECLARE_NODE(LoadSourceLocation);

#if !defined(ENABLE_HARKIO3)

/*Node
 *
 * @name LoadSourceLocation
 * @category HARK:Localization
 * @description Load source locations from a file with the "Source" format.
 *
 * @output_name SOURCES
 * @output_type Vector<ObjectRef>
 * @output_description Source locations with IDs. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name NOT_EOF
 * @output_type bool
 * @output_description True if the current frame does not reach the End-of-File.
 *
 * @parameter_name FILENAME
 * @parameter_type string
 * @parameter_description The filename to load source locations.
 *
 END*/

class LoadSourceLocation : public BufferedNode {
  int sourcesID;
  int noteofID;

  string filename;

  ifstream fin;
  int read_count;

public:
  LoadSourceLocation(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    sourcesID = addOutput("SOURCES");
    noteofID = addOutput("NOT_EOF");

    filename = object_cast<string>(parameters.get("FILENAME"));
    if (filename.c_str() == '\0') {
      throw new NodeException(NULL, string("FILENAME is empty."), __FILE__, __LINE__);
    }

    fin.open(filename.c_str());
    if(fin.fail())
      {
        throw new NodeException(NULL, string("Can't open '")+filename+string("'."), __FILE__, __LINE__);
      }

    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    Vector<ObjectRef>& sources = *new Vector<ObjectRef>;
    (*(outputs[sourcesID].buffer))[count] = &sources;
    string buffer;
    vector<string> tokens;

    getline(fin, buffer);
    (*(outputs[noteofID].buffer))[count] = (fin.eof() ? FalseObject : TrueObject);

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer_space;
    boost::char_separator<char> sep(" ");
    tokenizer_space tokenizer(buffer, sep);
    for (tokenizer_space::iterator it = tokenizer.begin();
         it != tokenizer.end(); ++it) {
      tokens.push_back(*it);
    }
    int source_count = 0;
    if ((tokens.size() - 1) % 4 == 0) {
      source_count = (tokens.size() - 1) / 4;
      if (lexical_cast<int>(tokens[0]) != count) {
        throw new NodeException(this, "The input of source location data is not correct.",
                                __FILE__, __LINE__);
      }
    }

    for (int i = 0; i < source_count; i++) {
      RCPtr<Source> src(new Source);

      src->power = 100.0f;
      src->id = lexical_cast<int>(tokens[i * 4 + 1]);
      src->x[0] = lexical_cast<float>(tokens[i * 4 + 2]);
      src->x[1] = lexical_cast<float>(tokens[i * 4 + 3]);
      src->x[2] = lexical_cast<float>(tokens[i * 4 + 4]);
      src->compare_mode = Source::DEG;
      sources.push_back(src);
    }

  }
};

#else // #if !defined(ENABLE_HARKIO3)

#include <string.h>
#include "libharkio3.h"


/*Node
 *
 * @name LoadSourceLocation
 * @category HARK:Localization
 * @description Load source locations from a file with the "Source" format.
 *
 * @output_name SOURCES
 * @output_type Vector<ObjectRef>
 * @output_description Source locations with IDs. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name NOT_EOF
 * @output_type bool
 * @output_description True if the current frame does not reach the End-of-File.
 *
 * @parameter_name FILENAME
 * @parameter_type string
 * @parameter_description The filename to load source locations.
 *
 END*/

class LoadSourceLocation : public BufferedNode {
  int sourcesID;
  int noteofID;

  string filename;

  ifstream fin;
  int read_count;

public:
  LoadSourceLocation(string nodeName, ParameterSet params): 
    BufferedNode(nodeName, params),
    read_count(0)
  {
    sourcesID = addOutput("SOURCES");
    noteofID = addOutput("NOT_EOF");

    filename = object_cast<string>(parameters.get("FILENAME"));
    if (filename.c_str() == '\0') {
      throw new NodeException(NULL, string("FILENAME is empty."), __FILE__, __LINE__);
    }

    fin.open(filename.c_str());
    if(fin.fail()) {
      throw new NodeException(NULL, string("Can't open '")+filename+string("'."), __FILE__, __LINE__);
    }

    inOrder = true;
  }

  ~LoadSourceLocation(void) {
    fin.close();
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    string buffer;

    Vector<ObjectRef>& sources = *new Vector<ObjectRef>;
    (*(outputs[sourcesID].buffer))[count] = &sources;

    (*(outputs[noteofID].buffer))[count] = TrueObject;

    stringstream ss;

    getline(fin, buffer);
    if (fin.eof()) {
      (*(outputs[noteofID].buffer))[count] = FalseObject;
      return;
    }

    size_t start;
    while ((start = buffer.find("<positions", 0)) == string::npos) {
      getline(fin, buffer, '>');
      buffer = buffer + '>';
      // cerr << "[1]" << endl << buffer << endl;
      if (fin.eof()) {
        (*(outputs[noteofID].buffer))[count] = FalseObject;
        return;
      }
    }
    string startbuf = buffer.substr(start);

    ss << startbuf.c_str();
    getline(fin, buffer, '>');
    buffer = buffer + '>';
    // cerr << "[2]" << endl << buffer << endl;
    if (fin.eof()) {
      (*(outputs[noteofID].buffer))[count] = FalseObject;
      return;
    }
    
    size_t end;
    while ((end = buffer.find("</positions>", 0)) == string::npos) {
      ss << buffer.c_str();
      getline(fin, buffer, '>');
      buffer = buffer + '>';
      // cerr << "[3]" << endl << buffer << endl;
      if (fin.eof()) {
        (*(outputs[noteofID].buffer))[count] = FalseObject;
        return;
      }
    }

    DEBUG_PRINT("endpos: %d", end);
    string endbuf = buffer.substr(0, end + strlen("</positions>") + 1);
    ss << endbuf.c_str();

    DEBUG_PRINT("abstract: %s", ss.str().c_str());

    string xmlbuf = ss.str();
    /*
    cerr << "BUFFER[" << endl;
    cerr << xmlbuf << endl;
    cerr << "]" << endl;
    */
    harkio_Positions* poses = harkio_Positions_newFromXMLbuffer((char*)xmlbuf.c_str(), xmlbuf.length(), NULL);

    //harkio_Positions_print(poses);
    for (int i = 0; i < poses->size; i++) {
      RCPtr<Source> src(new Source);
      harkio_Position* pos = poses->pos[i];

      src->power = 100.0f;
      src->id = harkio_Position_getID(pos);
      float* poses = harkio_Position_getPosition(pos);
      src->x[0] = poses[0];
      src->x[1] = poses[1];
      src->x[2] = poses[2];
      src->compare_mode = Source::DEG;

      sources.push_back(src);
    }
    read_count++;
  }
};

#endif // #if !defined(ENABLE_HARKIO3)
