/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Map.h"

using namespace std;
using namespace FD;

class MFMGeneration;

DECLARE_NODE(MFMGeneration)
/*Node
 *
 * @name MFMGeneration
 * @category HARK:MFM
 * @description Generate missing feature masks using information from sound source separation.
 *
 * @input_name FBANK
 * @input_type Map<int,ObjectRef>
 * @input_description Mel-scale spectrum of separated sounds. The key is sounce ID, and the value is a mel-scale spectrum (Vector<float>).
 *
 * @input_name FBANK_SS
 * @input_type Map<int,ObjectRef>
 * @input_description Mel-scale spectrum of the separated sound (Vector<float>).
 *
 * @input_name FBANK_BN
 * @input_type Map<int,ObjectRef>
 * @input_description Mel-scale spectrum of background noise (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Output mask vectors (Vector<float>). The static part of a feature vector (i.e., the first FBANK_COUNT elements) is calculated, and the delta part (the same size as the static part) is padded with 0.
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 13
 * @parameter_description Size of the static part of a feature vector. [default: 13]
 *
 * @parameter_name THRESHOLD
 * @parameter_type float
 * @parameter_value 0.2
 * @parameter_description Threshold for missing feature mask generation. The threshold should be between 0 and 1. [default: 0.2]
 *
 END*/

class MFMGeneration : public BufferedNode {
	int fbankID;
    int fbankSSID;
    int fbankBNID;
    int outputID;

    int fbank_count;
    float threshold;

    vector<int> ids;
    vector<RCPtr<Vector<float> > > fbanks;
    vector<RCPtr<Vector<float> > > fbanks_ss;
    vector<RCPtr<Vector<float> > > fbanks_bn;

public:
    MFMGeneration(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{
        fbankID = addInput("FBANK");
        fbankSSID = addInput("FBANK_SS");
        fbankBNID = addInput("FBANK_BN");
        outputID = addOutput("OUTPUT");

        fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));
        threshold = dereference_cast<float>(parameters.get("THRESHOLD"));

        //inOrder = true;
    }

	void calculate(int output_id, int count, Buffer &out)
	{
        RCPtr<Map<int, ObjectRef> > fbank_ptr = getInput(fbankID, count);
        const Map<int, ObjectRef>& fbank = *fbank_ptr;
        RCPtr<Map<int, ObjectRef> > fbank_ss_ptr = getInput(fbankSSID, count);
        const Map<int, ObjectRef>& fbank_ss = *fbank_ss_ptr;
        RCPtr<Map<int, ObjectRef> > fbank_bn_ptr = getInput(fbankBNID, count);
        const Map<int, ObjectRef>& fbank_bn = *fbank_bn_ptr;

        Buffer& outputBuffer = *(outputs[outputID].buffer);
        
        Map<int,ObjectRef>& output = *new Map<int,ObjectRef>;
        outputBuffer[count] = &output;

        ids.resize(0);
        fbanks.resize(0);
        fbanks_ss.resize(0);
        fbanks_bn.resize(0);
        // TODO: I had better check out ID and size of fbank,
        //       fbank_ss, and fbank_bn.
        Map<int, ObjectRef>::const_iterator it;
        int n;
        for (it = fbank.begin(); it != fbank.end(); ++it) {
            ids.push_back(it->first);
            fbanks.push_back(it->second);
        }
        for (n = 0, it = fbank_ss.begin(); it != fbank_ss.end() && n < fbanks.size();
             ++it) {
            if (ids[n] == it->first) {
                fbanks_ss.push_back(it->second);
                n++;
            }
        }
        for (n = 0, it = fbank_bn.begin(); it != fbank_bn.end() && n < fbanks.size();
             ++it) {
            if (ids[n] == it->first) {
                fbanks_bn.push_back(it->second);
                n++;
            }
        }

        for (int n = 0; n < fbanks.size(); n++) {
            Vector<float>& mask = *Vector<float>::alloc(fbank_count * 2);
            output[ids[n]] = &mask;

            for (int i = 0; i < fbank_count; i++) {
                float m = min(1.0f, (((*fbanks[n])[i] + 1.4f * (*fbanks_bn[n])[i]) /
                                     ((*fbanks_ss[n])[i] + 1.0f)));
                //float m = 1.0 - min(1.0f, (*fbanks_bn[n])[i] / (*fbanks_ss[n])[i]);

                if (m > threshold) {
                    mask[i] = 1.0f;
                }
                else {
                    mask[i] = 0.0f;
                }
            }
        }
    }

    IN_ORDER_NODE_SPEEDUP(MFMGeneration);
};


