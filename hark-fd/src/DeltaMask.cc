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

class DeltaMask;

DECLARE_NODE(DeltaMask)
/*Node
 *
 * @name DeltaMask
 * @category HARK:MFM
 * @description Calculate a delta mask vector from a sequence of mask vectors. The calculated delta mask vector is added to the end of the input mask vector. If the size of the input vector is N, the size of the output vector will become 2N.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Mask vectors. The key is a source ID, and the value is a mask vector (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Output mask vectors including the calculated delta masks (Vector<float>).
 *
 * @parameter_name FBANK_COUNT
 * @parameter_type int
 * @parameter_description The size of the input mask vector.
 *
 END*/

class DeltaMask : public BufferedNode {
	int inputID;
    int outputID;

    int fbank_count;
    int del_win;
    int B;
    vector<float> A1;
    vector<float> A2;
    vector<Map<int, ObjectRef>::iterator> it;

public:
    DeltaMask(string nodeName, ParameterSet params)
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
        it.resize(1 + del_win * 2);

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

        for (int i = 0; i < it.size(); i++) {
            it[i] = (*input[i]).begin();
        }

        for (; it[del_win] != (*input[del_win]).end(); ++it[del_win]) {
            int id = it[del_win]->first;
            Vector<float>& mask = object_cast<Vector<float> >(it[del_win]->second);
            for (int n = fbank_count; n < fbank_count * 2; n++) {
                mask[n] = 1.0f;
            }            

            for (int n = 0; n < fbank_count; n++) {
                A1[n] = A2[n] = mask[n];
            }

            // TODO: If I use RCPtr, below codes will become faster.
            for (int i = 1; i <= del_win; i++) {
                int t = del_win - i;
                while (it[t] != (*input[t]).end() && it[t]->first < id) {
                    ++it[t];
                }
                if (it[t] != (*input[t]).end() && it[t]->first == id) {
                    const Vector<float>& mask =
                        object_cast<Vector<float> >(it[t]->second);
                    
                    for (int n = 0; n < fbank_count; n++) {
                        A1[n] = mask[n];
                    }
                }

                t = del_win + i;
                while (it[t] != (*input[t]).end() && it[t]->first < id) {
                    ++it[t];
                }
                if (it[t] != (*input[t]).end() && it[t]->first == id) {
                    const Vector<float>& mask =
                        object_cast<Vector<float> >(it[t]->second);

                    for (int n = 0; n < fbank_count; n++) {
                        A2[n] = mask[n];
                    }
                }

                for (int n = 0; n < fbank_count; n++) {
                    mask[fbank_count + n] *= A1[n] * A2[n];
                }
            }
        }
    }

    IN_ORDER_NODE_SPEEDUP(DeltaMask);

};


