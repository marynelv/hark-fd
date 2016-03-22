/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#include "BufferedNode.h"
#include "Buffer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Map.h"
#include "Source.h"
#include <math.h>

#include <sys/time.h>

#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace FD;

class SemiBlindICA;

DECLARE_NODE(SemiBlindICA)
/*Node
 *
 * @name SemiBlindICA
 * @category HARK:MISC
 * @description Known noise separation using ICA.
 *
 * @input_name INPUT
 * @input_type Matrix<complex<float> >
 * @input_description Input: 1 frame spectra after STFT.
 *
 * @input_name REFERENCE
 * @input_type Matrix<complex<float> >
 * @input_description Input: 1 frame, 1 channel known spectra after STFT.
 *
 * @output_name OUTPUT
 * @output_type Matrix<complex<float> >
 * @output_description Separated 1 frame spectra.
 *
 * @parameter_name CHANNEL
 * @parameter_type int
 * @parameter_value 1
 * @parameter_description Number of channels.
 *
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description Frame length (FFT window length).
 *
 * @parameter_name INTERVAL
 * @parameter_type int
 * @parameter_value 1
 * @parameter_description Number of filter interval for multi-rate repeating.
 *
 * @parameter_name TAP_LOWERFREQ
 * @parameter_type int
 * @parameter_value 8
 * @parameter_description The length of the filter, corresponding to the reverberation time, at 0 Hz.
 *
 * @parameter_name TAP_UPPERFREQ
 * @parameter_type int
 * @parameter_value 4
 * @parameter_description The length of the filter, corresponding to the reverberation time, at the Nyquist frequency.
 *
 * @parameter_name DECAY
 * @parameter_type float
 * @parameter_value 0.8
 * @parameter_description Controls the decay of the learning rate at each time tap of the filter. This accounts for the decay of the reverberation.
 *
 * @parameter_name MU_FILTER
 * @parameter_type float
 * @parameter_value 0.01
 * @parameter_description  Learning rate for the filter (tap) coefficients.
 *
 * @parameter_name MU_REFERENCE
 * @parameter_type float
 * @parameter_value 0.01
 * @parameter_description Learning rate for the normalization parameter of the reference signal.
 *
 * @parameter_name MU_UNKNOWNSIGNAL
 * @parameter_type float
 * @parameter_value 0.01
 * @parameter_description Learning rate for the normalization parameter of the unknown component in the input signal.
 *
 * @parameter_name IS_ZERO
 * @parameter_type float
 * @parameter_value 0.0001
 * @parameter_description skip the processing if the input-power is lower than IS_ZERO.
 *
 * @parameter_name FILE_FILTER_IN
 * @parameter_type string
 * @parameter_value -null
 * @parameter_description load initial values of filter.
 *
 * @parameter_name FILE_FILTER_OUT
 * @parameter_type string
 * @parameter_value -null
 * @parameter_description save values of filter
 *
 * @parameter_name OUTPUT_FREQ
 * @parameter_type int
 * @parameter_value 150
 * @parameter_description interval of the output(s)
 *
 END*/

// class of sub-band semi blind ICA
class SBSemiBlindICA {
    int id_i; // for debug
    int id_j; // for debug

    int taps;
    int idx;
    int itvl;
    unsigned long mask;  // for classical ring buffer

    float mu_s;
    float mu_n;

    float n_var;
    float r_var;

    float alpha;
    float is_zero;
    float FLOOR_VALUE;

    Vector<complex<float> > mu_f;
    Vector<complex<float> > buf_s;
    Vector<complex<float> > buf_ns;
    Vector<complex<float> > w;

public:
    SBSemiBlindICA(int taps, int itvl, float lambda,
		   float mu_s, float mu_v, float mu_n, float is_zero) {
	int size;

	// normalization
	mu_s /= 10000;//36768.0;
	mu_v /= 10000;//36768.0;
	mu_n /= 10000;//36768.0;

	//printf("%d %d\n", i, j);
	//id_i = i;
	//id_j = j;

	// set member parameters
	this->itvl = itvl;
	this->taps = taps;
	this->mu_s = mu_s;
	this->mu_n = mu_n;
	this->is_zero = is_zero;

	idx  = 0;
	alpha = 1.0;
	n_var = 1.0;
	r_var = 1.0;
	FLOOR_VALUE = 1.0E-5;

	// filter parameter and stepsizes
	w.resize(taps);
	mu_f.resize(taps);

	// Exponentially weighted stepsize
	for (int i = 0; i < taps; i++) {
	    mu_f[i] = pow(lambda, i) * mu_v;
	}

	// for ring buffer
	for (int i = 1; i < 32; i++) {
	    size = 1 << i;
	    if ((taps * itvl) <= size)
		break;
	}
	mask = size - 1;

	buf_s.resize(size);
	buf_ns.resize(size);
    }

    // for debug
    void print() {
	printf("tap: %d, mu_s: %f, mu_n: %f\n",
	       taps, mu_s, mu_n);
    }

    //
    void save(FILE *fp) {
	float c[2];

	for (int i = 0; i < taps; i++) {
	    c[0] = w[i].real();
	    c[1] = w[i].imag();
	    fwrite(c, sizeof(float), 2, fp);
	}
    }

    //
    void load(FILE *fp) {
	float c[2];
	for (int i = 0; i < taps; i++) {
	    fread(c, sizeof(float), 2, fp);
	    w[i] = complex<float>(c[0], c[1]);
	}
    }

    // filtering
    complex<float> update(complex<float> x, complex<float> s) {
	complex<float> d, sum, c1, n_d;
	int midx;
	float ry, v1, e1, rx, v2, e2;

	//printf("%d/%d, %d/%d\n", ii, id_i, jj, id_j);

	//
	buf_s[idx] = s;

	if (abs(s) < is_zero) {
	    return x;
	}

	//
	sum = 0;
	for (int i = 0; i < taps; i++) {
	    midx = (idx - i * itvl) & mask;
	    sum += w[i] * buf_s[midx];
	}
	d = x - sum;

	//
	n_d = n_var * d;
	buf_ns[idx] = r_var * buf_s[idx];

    // Fixed value
    srand(1234);

	//
	ry = abs(n_d) + FLOOR_VALUE * rand()/ (double)RAND_MAX;
	v1 = tanh(alpha * ry);
	e1 = ry;
	c1 = v1 * n_d / ry;

	//
	rx = abs(buf_ns[idx]) + FLOOR_VALUE * rand()/(double)RAND_MAX;
	v2 = tanh(alpha * rx);
	e2 = rx;

	// filter update
	for (int i = 0; i < taps; i++) {
	    midx = (idx - i * itvl) & mask;
	    w[i] += mu_f[i] * (c1 * conj(buf_ns[midx]));
	}

	// variance estimation
	n_var += mu_n * (1 - v1 * e1) * n_var;
	if (n_var < 0) n_var *= -1;
	r_var += mu_s * (1 - v2 * e2) * r_var;
	if (r_var < 0) r_var *= -1;

	idx = (idx + 1) & mask;
	return d;
    }
};

// full-band semi blind ICA
class SemiBlindICA : public BufferedNode {
    string filter_in_str;
    string filter_out_str;

    const char *filter_in_chr;
    const char *filter_out_chr;

    FILE *filter_in;
    FILE *filter_out;

    int counter;
    int counter_loop;
    int freq;

    int inputID1;
    int inputID2;
    int outputID;

    int length;
    int nchannel;
    int tap_lower;
    int tap_upper;
    int itvl;

    float mu_v;
    float mu_s;
    float mu_n;
    float lambda;
    float is_zero;

    //vector<SBSemiBlindICA*> sbsbica;
    vector<vector<SBSemiBlindICA*> > sbsbica;

public:
    SemiBlindICA(string nodeName, ParameterSet params)
	: BufferedNode(nodeName, params) {
	inputID1 = addInput("INPUT");
	inputID2 = addInput("REFERENCE");
	outputID = addOutput("OUTPUT");

	nchannel  = dereference_cast<int> (parameters.get("CHANNEL"));
	length    = dereference_cast<int> (parameters.get("LENGTH"));
	itvl      = dereference_cast<int> (parameters.get("INTERVAL"));
	tap_lower = dereference_cast<int> (parameters.get("TAP_LOWERFREQ"));
	tap_upper = dereference_cast<int> (parameters.get("TAP_UPPERFREQ"));
	counter_loop = dereference_cast<int> (parameters.get("OUTPUT_FREQ"));

	mu_s = dereference_cast<float> (parameters.get("MU_REFERENCE"));
	mu_v = dereference_cast<float> (parameters.get("MU_FILTER"));
	mu_n = dereference_cast<float> (parameters.get("MU_UNKNOWNSIGNAL"));
	lambda = dereference_cast<float> (parameters.get("DECAY"));
	is_zero = dereference_cast<float> (parameters.get("IS_ZERO"));

	filter_in_str  = object_cast<string>(parameters.get("FILE_FILTER_IN"));
	filter_out_str = object_cast<string>(parameters.get("FILE_FILTER_OUT"));


	if (filter_in_str == "-null") {
	    filter_in_chr = NULL;
	}
	else {
	    filter_in_chr = filter_in_str.c_str();
	}
	if (filter_out_str == "-null") {
	    filter_out_chr = NULL;
	}
	else {
	    filter_out_chr = filter_out_str.c_str();
	}

	counter = 0;
	freq = length/2+1;

	filter_in = fopen(filter_in_chr, "rb");
	if (filter_in != NULL) {
	    if (read_header(filter_in) != 0) {
		printf("Error from `SemiBlindICA' Module\n");
		printf("\t file format error. exit.\n");
		exit(0);
	    }
	}

	sbsbica.resize(nchannel);
	for (int j = 0; j < nchannel; j++) {
	    for (int i = 0; i < freq; i++) {
		// linear interpolation of tap length
		int taps = (int)((double)tap_lower
				 + (double)(tap_upper - tap_lower)
				 * (double)i / (double)freq);
		// add element
		sbsbica[j].push_back(new SBSemiBlindICA(taps, itvl, lambda,
							mu_s, mu_v, mu_n,
							is_zero));
		if (filter_in != NULL) {
		    sbsbica[j][i]->load(filter_in);
		}
	    }
	}
	if (filter_in != NULL)
	    fclose(filter_in);
	//printf("a\n");

    }

    ~SemiBlindICA() {
	for (int j = 0; j < nchannel; j++) {
	    for (int i = 0; i < freq; i++) {
		delete sbsbica[j][i];
	    }
	}
    }

    virtual void initialize() {
	this->BufferedNode::initialize();
    }

    int read_header(FILE *fp) {
	int value;

	fread(&value, sizeof(int), 1, fp);
	if (value != nchannel) return -1;
	fread(&value, sizeof(int), 1, fp);
	if (value != freq) return -1;
	fread(&value, sizeof(int), 1, fp);
	if (value != tap_upper) return -1;
	fread(&value, sizeof(int), 1, fp);
	if (value != tap_lower) return -1;
	return 0;
    }

    void write_header(FILE *fp) {
	fwrite(&nchannel, sizeof(int), 1, fp);
	fwrite(&freq, sizeof(int), 1, fp);
	fwrite(&tap_upper, sizeof(int), 1, fp);
	fwrite(&tap_lower, sizeof(int), 1, fp);
    }

    void calculate(int output_id, int count, Buffer &out) {
	int col, row;

	RCPtr < Matrix <complex <float> > > input1 = getInput(inputID1, count);
	RCPtr < Matrix <complex <float> > > input2 = getInput(inputID2, count);
	row = input1->nrows();
	col = input1->ncols();
	RCPtr < Matrix <complex <float> > > output(new Matrix < complex < float > >(row, col));

	// update filter
	for (int ch = 0; ch < nchannel; ch++) {
	    for (int w = 0; w < freq; w++) {
		(*output)(ch, w) = sbsbica[ch][w]->update(
		    (*input1)(ch,w),
		    (*input2)(0,w));
	    }
	}
	out[count] = output;

	// save filter
	counter++;
	if (filter_out_chr != NULL && counter >= counter_loop) {
	    filter_out = fopen(filter_out_chr, "wb");
	    write_header(filter_out);
	    for (int ch = 0; ch < nchannel; ch++) {
		for (int w = 0; w < freq; w++) {
		    sbsbica[ch][w]->save(filter_out);
		}
	    }
	    fclose(filter_out);
	    counter = 0;
	}

    }

    IN_ORDER_NODE_SPEEDUP(SemiBlindICA);

};
