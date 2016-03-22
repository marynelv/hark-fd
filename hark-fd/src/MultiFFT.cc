/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#include <cstdlib>
#include <cstring>
#include <algorithm>

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Matrix.h>
#include <Vector.h>
#include <Stream.h>
#include "Map.h"

#include "fft.hpp"

using namespace FD;
using namespace std;
using namespace boost::numeric;

class MultiFFT;

DECLARE_NODE(MultiFFT)
/*Node
 *
 * @name MultiFFT
 * @category HARK:MISC
 * @description Fast Fourier Transform for multi-channel audio signals.
 *
 * @input_name INPUT
 * @input_type any
 * @input_description Input should be Matrix<float> or Map<int,ObjectRef>. In case of Matrix<float>, the rows should be channel indices and the columns should be frequency indices. In case of Map<int,ObjectRef>, the key is a source ID and the value is a vector of audio signals (Vector<float>).
 *
 * @output_name OUTPUT
 * @output_type any
 * @output_description When input is Matrix<float>, output is Matrix<complex<float> >. When input is Map<int,ObjectRef> whose ObjectRef is Vector<float>, output is Map<int,ObjectRef> whose ObjectRef is Vector<complex<float> >.
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description FFT length in sample. [default: 512]
 *
 * @parameter_name WINDOW
 * @parameter_type string
 * @parameter_value CONJ
 * @parameter_list CONJ:HAMMING:RECTANGLE:HANNING
 * @parameter_description A window function for FFT. WINDOW should be CONJ, HAMMING, RECTANGLE, or HANNING. [default: CONJ]
 *
 * @parameter_name WINDOW_LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description Window length of the window function. [default: 512]
 *
 END*/

enum WINDOW_TYPE {
  CONJ_WINDOW, RECTANGLE_WINDOW, HAMMING_WINDOW, HANNING_WINDOW
};


class MultiFFT : public BufferedNode {
  int inputID;
  int outputID;
  int length;
  int window_type;

  int pslength;
  int r;

  typedef Matrix<float> FMatrix;
  typedef Matrix<complex<float> > FCMatrix;

  ublas::vector<float> window;
  rfft<float> rfft1;
    
public:
  MultiFFT(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");
        
    length = dereference_cast<int>(parameters.get("LENGTH"));
    pslength = length / 2 + 1;
    r = (int)(log2(length) + 0.5);
    string window_name = object_cast<String>(parameters.get("WINDOW"));
    if (window_name == "CONJ") {
      window_type = CONJ_WINDOW;
    }
    else if (window_name == "RECTANGLE") {
      window_type = RECTANGLE_WINDOW;
    }
    else if (window_name == "HAMMING") {
      window_type = HAMMING_WINDOW;
    }
    else if (window_name == "HANNING") {
      window_type = HANNING_WINDOW;
    }
    else {
      throw new NodeException(NULL,
                              "MultiFFT: WINDOW should be "
                              "\"CONJ\", \"RECTANGLE\" or \"HAMMING\".",
                              __FILE__, __LINE__);
    }
    int window_length = dereference_cast<int>(parameters.get("WINDOW_LENGTH"));
    if (window_length > length) {
      throw new NodeException(NULL, "MultiFFT: WINDOW_LENGTH should be <= LENGTH.",
                              __FILE__, __LINE__);
    }

    window.resize(window_length);
    switch (window_type) {
    case CONJ_WINDOW:
      ConjWindow(window);
      break;
    case RECTANGLE_WINDOW:
      for (int i = 0; i < window.size(); i++) {
        window[i] = 1.0f;
      }
      break;
    case HAMMING_WINDOW:
      Hamming(window);
      break;
    case HANNING_WINDOW:
      Hanning(window);
      break;
    }
    window.resize(length);
		
    rfft1.initialize(length);
		
    inOrder = true;
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    ObjectRef in = getInput(inputID, count);

    if (typeid(*in) == typeid(FMatrix)) {
      RCPtr<FMatrix> input = getInput(inputID, count);
      RCPtr<FCMatrix> output(new FCMatrix(input->nrows(), pslength));

      int nchan = input->nrows();

      //cout << count << endl;
      if (input->ncols() != length) {
        throw new NodeException(this, "MultiFFT: The FFT length and the column number of the matrix do not match.", __FILE__, __LINE__);
      }
            
      for (int m = 0; m < input->nrows(); m++) {
        for (int i = 0; i < input->ncols(); i++) {
          rfft1.a[i] = (*input)(m, i) * window[i];
        }
        rfft1.analyze();
        for (int k = 0; k < output->ncols(); k++) {
          (*output)(m, k) = rfft1.A[k];
        }
      }
      //cout << (*input) << endl;
      //cout << (*output) << endl;

      out[count] = output;
    }
    else if (typeid(*in) == typeid(Map<int, ObjectRef>)) {
      RCPtr<Map<int, ObjectRef> > input = in;
      RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>);

      Map<int, ObjectRef>::iterator it = input->begin();
      for (; it != input->end(); ++it) {
        Vector<float>& x = object_cast<Vector<float> >(it->second);
        RCPtr<Vector<complex<float> > >
          X(new Vector<complex<float> >(rfft1.flength(), 0.0f));
        for (int i = 0; i < x.size(); i++) {
          rfft1.a[i] = x[i] * window[i];
        }
        rfft1.analyze();
        for (int k = 0; k < X->size(); k++) {
          (*X)[k] = rfft1.A[k];
        }
        (*output)[it->first] = X;
      }
      out[count] = output;
    }
    else {
      cerr << "Error\n";
    }
  }

  NO_ORDER_NODE_SPEEDUP(MultiFFT);
};
