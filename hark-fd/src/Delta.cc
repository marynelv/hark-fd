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
#include <assert.h>

using namespace std;
using namespace FD;

class Delta;

DECLARE_NODE(Delta)
/*Node
 *
 * @name Delta
 * @category HARK:FeatureExtraction
 * @description Calculate a delta feature vector from a sequence of feature vectors. The calculated delta feature vector is added to the end of the input feature vector. If the size of the input vector is N, the size of the output vector will become 2N.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Input feature vector. The key is a source ID, and the value is a feature vector (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Output feature vector including calcuated delta features.
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_value 13
 * @parameter_description The size of the input feature vector.
 *
 END*/

class Delta : public BufferedNode {
	int inputID;
    int outputID;

    int fbank_count;
    int del_win;
    int B;
    vector<float> A1;
    vector<float> A2;
    vector<float> sum;

public:
    Delta(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{
        inputID = addInput("INPUT");
        outputID = addOutput("OUTPUT");

        fbank_count = dereference_cast<int>(parameters.get("FBANK_COUNT"));

        del_win = 2;
        
        inputsCache[inputID].lookAhead = del_win;
        inputsCache[inputID].lookBack = del_win;
        
        inOrder = true;

        A1.resize(fbank_count);
        A2.resize(fbank_count);
        sum.resize(fbank_count);

        B = 0;
        for (int i = 1; i <= del_win; i++) {
            B += i * i;
        }
    }

	void calculate(int output_id, int count, Buffer &out)
	{
        vector<RCPtr<Map<int, ObjectRef> > > input(1 + del_win * 2);
        
        input[del_win] = getInput(inputID, count);
        for (int i = 1; i <= del_win; i++) {
            input[del_win + i] = getInput(inputID, count + i);

            if (count - i >= 0) {
                input[del_win - i] = getInput(inputID, count - i);
            }
            else {
                input[del_win - i] = input[del_win - i + 1];
            }
        }

        Buffer& outputBuffer = *(outputs[outputID].buffer);
        outputBuffer[count] = input[del_win];

        Map<int, ObjectRef>::iterator cur_f;
        for (cur_f = input[del_win]->begin();
             cur_f != input[del_win]->end(); ++cur_f) {
            int id = cur_f->first;
            Vector<float>& fbank =
                object_cast<Vector<float> >(cur_f->second);
            for (int n = 0; n < fbank_count; n++) {
                A1[n] = A2[n] = fbank[n];
            }

            sum.assign(fbank_count, 0.0f);
            Map<int, ObjectRef>::iterator it;
            for (int i = 1; i <= del_win; i++) {
                for (int t = del_win - i; t <= del_win; t++) {
                    if ((it = input[t]->find(id)) != input[t]->end()) {
                        break;
                    }
                    assert(t != del_win);
                }

                const Vector<float>& fbank_bw =
                    object_cast<Vector<float> >(it->second);
                for (int n = 0; n < fbank_count; n++) {
                    A1[n] = fbank_bw[n];
                }

                for (int t = del_win + i; t >= del_win; t--) {
                    if ((it = input[t]->find(id)) != input[t]->end()) {
                        break;
                    }
                    assert(t != del_win);
                }
                const Vector<float>& fbank_fw =
                    object_cast<Vector<float> >(it->second);
                
                for (int n = 0; n < fbank_count; n++) {
                    A2[n] = fbank_fw[n];
                }

                for (int n = 0; n < fbank_count; n++) {
                    sum[n] += i * (A2[n] - A1[n]);
                }
            }
            
            for (int n = 0; n < fbank_count; n++) {
                fbank[fbank_count + n] = sum[n] / (2 * B);
            }
        }
    }

    IN_ORDER_NODE_SPEEDUP(Delta);

};


