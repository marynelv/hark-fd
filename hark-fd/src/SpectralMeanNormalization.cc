/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include <boost/bind.hpp>

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"
#include "Source.h"

//#include "fft.hpp"
#include "SpectralMean.hpp"

using namespace std;
using namespace boost;
using namespace FD;

class SpectralMeanNormalization;

DECLARE_NODE(SpectralMeanNormalization)
/*Node
 *
 * @name SpectralMeanNormalization
 * @category HARK:FeatureExtraction
 * @description Perform Spectral Mean Normalization (SMN). A spectral mean is calculated per direction (resolution: 10 degrees).
 *
 * @input_name FBANK
 * @input_type Map<int,ObjectRef>
 * @input_description Input feature vectors. The key is source ID, and the value is a feature vector (Vector<float>).
 *
 * @input_name SOURCES
 * @input_type Vector<ObjectRef>
 * @input_description Source locations with ID. Each element of the vector is a source location with ID specified by "Source".
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Feature vectors after SMN.
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 13
 * @parameter_description Size of a static part of a feature vector. [default: 13]
 *
 END*/

bool is_near(float x, float y, float d)
{
    float diff = x - y;
    return -d < diff && diff < d;
}


class SpectralMeanNormalization : public BufferedNode
{
  int m_fbankID;     /* Input node */
  int m_sourcesID;   /* Input node */
  int m_outputID;    /* Output node */
  int m_fbank_count; /* Parameter */
  /************************************/
  //int duration;
  //vector<float> lifter;
  vector<float>        angle_list;
  vector<SpectralMean> spec_mean_list;
  map<int, int>        id_angle_map;
  //bool                mean_is_stocked;
public:
    SpectralMeanNormalization(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
    {
        m_fbankID     = addInput("FBANK");
        m_sourcesID   = addInput("SOURCES");
        m_outputID    = addOutput("OUTPUT");
        m_fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));

        angle_list.resize(36);
        spec_mean_list.resize(36);
        for (int i = 0; i < 36; i++) {
            angle_list[i] = i * 10.0f <= 180.0f ? i * 10.0f : i * 10.0f - 360.0f;
            spec_mean_list[i].Initialize(m_fbank_count);
        }
        inOrder = true;
    }

    void calculate(int output_id, int count, Buffer &out)
    {
        RCPtr<Map<int, ObjectRef> > fbank_map = getInput(m_fbankID, count);
        const Map<int, ObjectRef>& fbanks = *fbank_map;
        RCPtr<Vector<ObjectRef> > sources = getInput(m_sourcesID, count);

        Buffer& outputBuffer = *(outputs[m_outputID].buffer);
        
        //Map<int,ObjectRef>& output = *new Map<int,ObjectRef>;
        //outputBuffer[count] = &output;
        outputBuffer[count] = fbank_map;

        for (int n = 0; n < sources->size(); n++) {
            RCPtr<Source> src = (*sources)[n];

            Map<int, ObjectRef>::iterator it = fbank_map->find(src->id);
            if (it == fbank_map->end()) {
                continue;
            }
            float angle = 180 / M_PI * atan2(src->x[1], src->x[0]);

            RCPtr<Vector<float> > fbank = it->second;
            if (fbank->size() < m_fbank_count) {
              throw new NodeException(this, "Size of input feature vector is less than FBANK_COUNT.", __FILE__, __LINE__);
            }

            map<int, int>::iterator it_id_angle = id_angle_map.find(src->id);
            int ind = 0;
            if (it_id_angle == id_angle_map.end()) {
                vector<float>::iterator it_angle = find_if(angle_list.begin(),
                                                           angle_list.end(),
                                                           bind(is_near,
                                                                _1, angle, 10.0f));
                ind = distance(angle_list.begin(), it_angle);
                id_angle_map[src->id] = ind;
            } else {
                ind = it_id_angle->second;
            }

            spec_mean_list[ind].AddSpectrum(*fbank);

            for (int i = 0; i < m_fbank_count; i++) {
                (*fbank)[i] -= spec_mean_list[ind].GetSpectralMean()[i];
                //if (angle_list[ind] == 40) {
                //cout << spec_mean_list[ind].GetSpectralMean()[i] << " ";
                //}
            }
            //if (angle_list[ind] == 40) {
            //cout << endl;
            //}
        }

        map<int, int>::iterator it;
        for (it = id_angle_map.begin(); it != id_angle_map.end();) {
            Map<int, ObjectRef>::iterator it_fbank = fbank_map->find(it->first);
            if (it_fbank == fbank_map->end()) {
                spec_mean_list[it->second].CalculateMean();
                spec_mean_list[it->second].Clear();
                id_angle_map.erase(it++);
            } else {
                ++it;
            }
        }
    }
    IN_ORDER_NODE_SPEEDUP(SpectralMeanNormalization);
};
