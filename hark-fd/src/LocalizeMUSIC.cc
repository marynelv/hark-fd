/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
 
#define NDEBUG

#include <iostream>
#include <BufferedNode.h>
#include <Buffer.h>
#include <Vector.h>
#include <Matrix.h>
#include <Map.h>
#include <math.h>
#include <Source.h>
#include <Tensor.h>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include "spline.hpp"
#include <deque> 

#include <boost/scoped_ptr.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include "../config.h"

#ifdef ENABLE_HARKIO
 
#ifndef ENABLE_HARKIO_NH

#include "LocalizationMUSIC.hpp"
#include "HarkParamsObjects.h"

using namespace std;
using namespace boost;
using namespace FD;
using namespace boost::numeric;
using namespace boost::posix_time;

class LocalizeMUSIC;

DECLARE_NODE(LocalizeMUSIC);
/*Node
 *
 * @name LocalizeMUSIC
 * @category HARK:Localization
 * @description Sound Source Localization by MUSIC
 *
 * @input_name INPUT
 * @input_type Matrix<complex<float> >
 * @input_description Multi-channel audio signals. In this matrix, a row is a channel, and a column is a sample.
 *
 * @input_name NOISECM
 * @input_type Matrix<complex<float> >
 * @input_description  Optional correlation matrix. This is a 2D matrix. The row denotes the frequency series.  The col denotes the correlation matrix of each frequency. The col should be transformed to a matrix in the receiver. If this is not connected, an identity matrix would be used.
 *
 * @output_name OUTPUT
 * @output_type Vector<ObjectRef>
 * @output_description Source location. A type of an element of the vector is Source.
 *
 * @parameter_name MUSIC_ALGORITHM
 * @parameter_type string
 * @parameter_value SEVD
 * @parameter_list SEVD:GEVD:GSVD
 * @parameter_description Sound Source Localization Algorithm. If SEVD, NOISECM will be ignored
 * 
 * @parameter_name TF_CHANNEL_SELECTION
 * @parameter_type object
 * @parameter_value <Vector<int> 0 1 2 3 4 5 6 7>
 * @parameter_description Microphone channels for localization
 * 
 * @parameter_name LENGTH
 * @parameter_type int
 * @parameter_value 512
 * @parameter_description The length of a frame (per channel).
 * 
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_description Sampling Rate (Hz).
 * 
 * @parameter_name A_MATRIX
 * @parameter_type string
 * @parameter_description Filename of a transfer function matrix.
 * 
 * @parameter_name WINDOW
 * @parameter_type int
 * @parameter_value 50
 * @parameter_description The number of frames used for calculating a correlation function.
 * 
 * @parameter_name WINDOW_TYPE
 * @parameter_type string
 * @parameter_list PAST:MIDDLE:FUTURE
 * @parameter_value FUTURE
 * @parameter_description Window selection to accumulate a correlation function. If PAST, the past WINDOW frames from the current frame are used for the accumulation. If MIDDLE, the current frame will be the middle of the accumulated frames. If FUTURE, the future WINDOW frames from the current frame are used for the accumulation. FUTURE is the default from version 1.0, but this makes a delay since we have to wait for the future information. PAST generates a internal buffers for the accumulation, which realizes no delay for localization. 
 * 
 * @parameter_name PERIOD
 * @parameter_type int
 * @parameter_value 50
 * @parameter_description The period in which the source localization is processed.
 * 
 * @parameter_name NUM_SOURCE
 * @parameter_type int
 * @parameter_value 2
 * @parameter_description Number of sources, which should be less than number of channels.
 * 
 * @parameter_name MIN_DEG
 * @parameter_type int
 * @parameter_description source direction (lower).
 * @parameter_value -180
 * 
 * @parameter_name MAX_DEG
 * @parameter_type int
 * @parameter_description source direction (higher).
 * @parameter_value 180
 * 
 * @parameter_name LOWER_BOUND_FREQUENCY
 * @parameter_type int
 * @parameter_value 500
 * @parameter_description Lower bound of frequency (Hz) used for correlation function calculation.
 * 
 * @parameter_name UPPER_BOUND_FREQUENCY
 * @parameter_type int
 * @parameter_value 2800
 * @parameter_description Upper bound of frequency (Hz) used for correlation function calculation.
 * 
 * @parameter_name SPECTRUM_WEIGHT_TYPE
 * @parameter_type string
 * @parameter_list Uniform:A_Characteristic:Manual_Spline:Manual_Square
 * @parameter_value Uniform
 * @parameter_description MUSIC spectrum weight for each frequency bin.
 *
 * @parameter_name A_CHAR_SCALING
 * @parameter_type float
 * @parameter_value 1.0
 * @parameter_valid SPECTRUM_WEIGHT_TYPE=A_Characteristic
 * @parameter_description Scaling factor of the A-Weight with respect to frequency
 * 
 * @parameter_name MANUAL_WEIGHT_SPLINE
 * @parameter_type object
 * @parameter_value <Matrix<float> <rows 2> <cols 5> <data 0.0 2000.0 4000.0 6000.0 8000.0 1.0 1.0 1.0 1.0 1.0> >
 * @parameter_valid SPECTRUM_WEIGHT_TYPE=Manual_Spline 
 * @parameter_description MUSIC spectrum weight for each frequency bin. This is a 2 by M matrix. The first row represents the frequency, and the second row represents the weight gain. "M" represents the number of key points for the spectrum weight. The frequency range between M key points will be interpolated by spline manner. The format is "<Matrix<float> <rows 2> <cols 2> <data 1 2 3 4> >". 
 *  
 * @parameter_name MANUAL_WEIGHT_SQUARE 
 * @parameter_type object 
 * @parameter_value <Vector<float> 0.0 2000.0 4000.0 6000.0 8000.0> 
 * @parameter_valid SPECTRUM_WEIGHT_TYPE=Manual_Square 
 * @parameter_description MUSIC spectrum weight for each frequency bin. This is a M order vector. The element represents the frequency points for the square wave. "M" represents the number of key points for the square wave weight. The format is "<Vector<float> 1 2 3 4>". 
 * 
 * @parameter_name ENABLE_EIGENVALUE_WEIGHT 
 * @parameter_type bool 
 * @parameter_value true 
 * @parameter_list true:false 
 * @parameter_description If true, the spatial spectrum is weighted depending on the eigenvalues of a correlation matrix. We do not suggest to use this function with GEVD and GSVD, because the NOISECM changes the eigenvalue drastically. Only useful for SEVD. 
 * 
 * @parameter_name ENABLE_INTERPOLATION 
 * @parameter_type bool 
 * @parameter_value false 
 * @parameter_list true:false 
 * @parameter_description Enable transfer function interpolation for higher resolution 
 *  
 * @parameter_name INTERPOLATION_TYPE 
 * @parameter_type string 
 * @parameter_list FDLI:TDLI:FTDLI 
 * @parameter_value FTDLI 
 * @parameter_valid ENABLE_INTERPOLATION=true 
 * @parameter_description Selection for interpolation algorithm 
 *  
 * @parameter_name HEIGHT_RESOLUTION 
 * @parameter_type float 
 * @parameter_value 1.0 
 * @parameter_valid ENABLE_INTERPOLATION=true 
 * @parameter_description Height resolution for the sound source localization 
 *  
 * @parameter_name AZIMUTH_RESOLUTION 
 * @parameter_type float 
 * @parameter_value 1.0 
 * @parameter_valid ENABLE_INTERPOLATION=true 
 * @parameter_description Azimuth resolution for the sound source localization 
 *  
 * @parameter_name RANGE_RESOLUTION 
 * @parameter_type float 
 * @parameter_value 1.0 
 * @parameter_valid ENABLE_INTERPOLATION=true 
 * @parameter_description Range (distance) resolution for the sound source localization
 * 
 * @parameter_name PEAK_SEARCH_ALGORITHM
 * @parameter_type string
 * @parameter_list LOCAL_MAXIMUM:HILL_CLIMBING
 * @parameter_value LOCAL_MAXIMUM
 * @parameter_description Algorithm for peak search in MUSIC spectrum. If LOCAL_MAXIMUM, the peak is defined only when the corresponding grid has the maximum spectrum among those of all adjacent grids. If HILL_CLIMBING, the peak is firstly searched in the azimuth and secondly searched in the elevation in the corresponding azimuth and thirdly searched in the radius in the corresponding azimuth and elevation.  
 * 
 * @parameter_name MAXNUM_OUT_PEAKS
 * @parameter_type int
 * @parameter_description Maximum number of output peaks. If MAXNUM_OUT_PEAKS = NUM_SOURCE, this is compatible with HARK version 1.0. If MAXNUM_OUT_PEAKS = 0, all local maxima are output. If MAXNUM_OUT_PEAKS < 0, MAXNUM_OUT_PEAKS is set to NUM_SOURCE. If MAXNUM_OUT_PEAKS > 0, number of output peaks is limited to MAXNUM_OUT_PEAKS.
 * @parameter_value -1
 *
 * @parameter_name DEBUG
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Debug option. If the parameter is true, this node outputs sound localization results to a standard output.
 *
END*/

class LocalizeMUSIC : public BufferedNode {

  int inputID;
  int paramsID;
  int tfnameID;
  int parammapID;
  int operationID;
  int noisecmID;

  int outputID;

  int spectrumID;
  int spectrumbinID;
  int tfinfoID;
  int rxxsID;
  int rxxnID;

  bool bspectrumID; 
  bool bspectrumbinID; 
  bool btfinfoID; 
  bool brxxsID; 
  bool brxxnID; 

  string music_algorithm;
  int nb_channels;
  Vector<int> channel_selection;
  vector<int> ch_selection;
  int length;
  int samplingRate;
  string FilenameAMatrix;
  int num_source;
  int mindeg;
  int maxdeg;
  int lower_bound;
  int upper_bound;
  float t_s;
  string spectrum_weight_type;
  Matrix<float> SSW;
  Vector<float> SSW_SQ;
  ublas::vector<float> MSSW;
  bool enable_eigenvalue_weight;
  bool enable_interpolation; 
  bool enable_hierarchical; 
  string interpolation_type; 
  float height_resolution; 
  float azimuth_resolution; 
  float range_resolution; 
  vector<float> resolution; 
  string peak_search_algorithm;
  int maxnum_out_peaks;
  bool is_debug;

  int pslength;
  fcmatrix x;
  deque<fcmatrix> xbuf;
  vector<fcmatrix> Rxx;
  vector<fcmatrix> RxxN;
  int sum_count;
  int period;
  int window;
  string window_type;
  int look_ahead;
  int look_back;

  int nh, nd, nr;

  string ptfname;

  int RxxNinit_flag;

  scoped_ptr<LocalizationMUSIC> localizer;

  RCPtr<Vector<ObjectRef> > tf_info;

  typedef Matrix<complex<float> > FCMatrix; // FlowdesignerÍÑ

public:
  LocalizeMUSIC(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params),
      paramsID(-1), tfnameID(-1), parammapID(-1), operationID(-1), 
      noisecmID(-1), spectrumID(-1), bspectrumID(false),
      spectrumbinID(-1), bspectrumbinID(false), tfinfoID(-1), btfinfoID(false),
      rxxsID(-1), brxxsID(false), rxxnID(-1), brxxnID(false) 
  {
    inputID = addInput("INPUT");
    outputID = addOutput("OUTPUT");

    music_algorithm = object_cast<string>(parameters.get("MUSIC_ALGORITHM"));
    channel_selection = object_cast<Vector<int> >(parameters.get("TF_CHANNEL_SELECTION"));      
    nb_channels = (int)channel_selection.size();
    ch_selection.resize(nb_channels);
    for(unsigned int i = 0; i < channel_selection.size(); i++) ch_selection[i] = channel_selection[i];
    length = dereference_cast<int>(parameters.get("LENGTH"));
    samplingRate = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
    FilenameAMatrix = object_cast<String>(parameters.get("A_MATRIX"));
    window = dereference_cast<int>(parameters.get("WINDOW"));
    window_type = object_cast<String>(parameters.get("WINDOW_TYPE"));
    period = dereference_cast<int>(parameters.get("PERIOD"));
    num_source = dereference_cast<int>(parameters.get("NUM_SOURCE"));
    mindeg = dereference_cast<int>(parameters.get("MIN_DEG"));
    maxdeg = dereference_cast<int>(parameters.get("MAX_DEG"));
    lower_bound = dereference_cast<int>(parameters.get("LOWER_BOUND_FREQUENCY"));
    upper_bound = dereference_cast<int>(parameters.get("UPPER_BOUND_FREQUENCY"));
    t_s = dereference_cast<float>(parameters.get("A_CHAR_SCALING"));
    spectrum_weight_type = object_cast<String>(parameters.get("SPECTRUM_WEIGHT_TYPE"));
    enable_eigenvalue_weight = dereference_cast<bool>(parameters.get("ENABLE_EIGENVALUE_WEIGHT"));
    enable_interpolation = dereference_cast<bool>(parameters.get("ENABLE_INTERPOLATION")); 
    if(enable_interpolation) interpolation_type = object_cast<String>(parameters.get("INTERPOLATION_TYPE")); 
    else interpolation_type = "None"; 
    height_resolution = dereference_cast<float>(parameters.get("HEIGHT_RESOLUTION")); 
    azimuth_resolution = dereference_cast<float>(parameters.get("AZIMUTH_RESOLUTION")); 
    range_resolution = dereference_cast<float>(parameters.get("RANGE_RESOLUTION")); 
    resolution.resize(3); 
    resolution[0] = height_resolution; 
    resolution[1] = azimuth_resolution; 
    resolution[2] = range_resolution; 
    enable_hierarchical = false; 
    peak_search_algorithm = object_cast<String>(parameters.get("PEAK_SEARCH_ALGORITHM"));
    maxnum_out_peaks = dereference_cast<int>(parameters.get("MAXNUM_OUT_PEAKS"));
    is_debug = dereference_cast<bool>(parameters.get("DEBUG"));

    pslength = length / 2 + 1;

    if(spectrum_weight_type == "Manual_Square"){ 
      SSW_SQ = object_cast<Vector<float> >(parameters.get("MANUAL_WEIGHT_SQUARE"));       
      MSSW.resize(pslength); 
      for(unsigned int i = 0; i < pslength; i++){ 
	int SSW_j = -1; 
	for(unsigned int j = 0; j < SSW_SQ.size(); j++){ 
	  if(SSW_SQ[j] <= samplingRate * (float)i / (float)length) SSW_j = j;   
	} 
	if(SSW_j % 2 == 0) MSSW[i] = 1.0; else MSSW[i] = 0.0; 
      } 
    } else { 
      if(spectrum_weight_type == "Manual_Spline"){ 
	SSW = object_cast<Matrix<float> >(parameters.get("MANUAL_WEIGHT_SPLINE")); 
      }else if(spectrum_weight_type == "Uniform"){ 
	SSW.resize(2,5); 
	SSW(0,0) = 0.0 / 4.0 / 2.0 * (float)samplingRate; 
	SSW(0,1) = 1.0 / 4.0 / 2.0 * (float)samplingRate; 
	SSW(0,2) = 2.0 / 4.0 / 2.0 * (float)samplingRate; 
	SSW(0,3) = 3.0 / 4.0 / 2.0 * (float)samplingRate; 
	SSW(0,4) = 4.0 / 4.0 / 2.0 * (float)samplingRate; 
	SSW(1,0) = 1.0; SSW(1,1) = 1.0; SSW(1,2) = 1.0; SSW(1,3) = 1.0; SSW(1,4) = 1.0;  
      }else if(spectrum_weight_type == "A_Characteristic"){ 
	SSW.resize(2,16); 
	SSW(0,0)   = 10.0 * t_s;   SSW(0,1)  = 12.5 * t_s;   SSW(0,2)  = 16.0 * t_s;    SSW(0,3)  = 20.0 * t_s; 
	SSW(0,4)  = 31.5 * t_s;    SSW(0,5) = 63.0 * t_s;    SSW(0,6)  = 125.0 * t_s;   SSW(0,7)  = 250.0 * t_s;   
	SSW(0,8)  = 500.0 * t_s;   SSW(0,9)  = 1000.0 * t_s; SSW(0,10) = 2000.0 * t_s;  SSW(0,11) = 4000.0 * t_s;  
	SSW(0,12) = 8000.0 * t_s; SSW(0,13) = 12500.0 * t_s; SSW(0,14) = 16000.0 * t_s; SSW(0,15) = 20000.0 * t_s;  
	SSW(1,0)   = pow(10.0,-70.4/20.0);  SSW(1,1)  = pow(10.0,-63.4/20.0);  SSW(1,2)  = pow(10.0,-56.7/20.0);    
	SSW(1,3)   = pow(10.0,-50.5/20.0);  SSW(1,4)  = pow(10.0,-39.4/20.0);  SSW(1,5) = pow(10.0,-26.2/20.0);  
	SSW(1,6)   = pow(10.0,-16.1/20.0);  SSW(1,7)  = pow(10.0,-8.6/20.0);   SSW(1,8)  = pow(10.0,-3.2/20.0);     
	SSW(1,9)   = pow(10.0,0.0/20.0);    SSW(1,10) = pow(10.0,1.2/20.0);    SSW(1,11)  = pow(10.0,1.0/20.0);   
	SSW(1,12)  = pow(10.0,-1.1/20.0);   SSW(1,13) = pow(10.0,-4.3/20.0);   SSW(1,14) = pow(10.0,-6.6/20.0);     
	SSW(1,15)  = pow(10.0,-9.3/20.0);        
      } 
      ublas::vector<float> KEY_X(SSW.ncols()); 
      ublas::vector<float> KEY_Y(SSW.ncols()); 
      for(unsigned int i = 0; i < SSW.ncols(); i++){ 
	KEY_X[i] = SSW(0,i); 
	KEY_Y[i] = SSW(1,i); 
      } 
      ublas::vector<float> SPLINE_X(pslength); 
      ublas::vector<float> SPLINE_Y(pslength); 
      for(unsigned int i = 0; i < SPLINE_X.size(); i++){ 
	SPLINE_X[i] = samplingRate * (float)i / (float)length; 
	if((SPLINE_X[i] < SSW(0,0))||(SPLINE_X[i] > SSW(0,SSW.ncols()-1))){ 
	  SPLINE_Y[i] = 0.0; 
	  continue; 
	} 
	SPLINE_Y[i] = cubic_spline(KEY_X, KEY_Y, SPLINE_X[i]); 
      } 
      MSSW.resize(pslength); 
      // noalias(MSSW) = SPLINE_Y / (float)ublas::sum(SPLINE_Y) * (float)pslength; 
      noalias(MSSW) = SPLINE_Y;
    } 

    x.resize(nb_channels, pslength);
    xbuf.resize(0);
    Rxx.resize(pslength);
    RxxN.resize(pslength);
    for (int k = 0; k < pslength; k++) {
      //Rxx[k].resize(nb_channels, nb_channels);
      Rxx[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
      RxxN[k] = ublas::zero_matrix<complex<float> >(nb_channels, nb_channels);
    }

    RxxNinit_flag = 0;

    if (num_source >= nb_channels) {
      throw new NodeException(NULL, string("Number of sources should be less than number of channels."), __FILE__, __LINE__);
    }

    localizer.reset(new LocalizationMUSIC(nb_channels, samplingRate,
                                          length, length, peak_search_algorithm, maxnum_out_peaks));
    cout << "reading A matrix. ch_selection: " << ch_selection << "\n";
    localizer->ReadTransferFunction(FilenameAMatrix.c_str(), ch_selection, enable_interpolation, enable_hierarchical, interpolation_type, resolution);
    cout << "done\n";
    localizer->SetNumSource(num_source);
    localizer->SetUsedFrequency(lower_bound, upper_bound);
    ptfname = FilenameAMatrix;

    nh = localizer->GetSearchHeight().size();
    nd = localizer->GetSearchDirection().size();
    nr = localizer->GetSearchRange().size();    

    tf_info = Vector<ObjectRef>::alloc(0);
    for (unsigned int i = 0; i < localizer->GetSearchDirection().size(); i++) {
      RCPtr<Source> src(new Source);
      src->id = i;
      src->power = 0.0;
      float theta = localizer->GetSearchDirection()[i] * M_PI / 180.0f;
      float r = localizer->GetSearchRange()[i];
      float phi = localizer->GetSearchHeight()[i] * M_PI / 180.0f;
      src->x[0] = r * cos(theta) * cos(phi);
      src->x[1] = r * sin(theta) * cos(phi);
      src->x[2] = r * sin(phi);
      tf_info->push_back(src);
    }

    if(window_type == "FUTURE"){
      look_ahead = window - 1;
      look_back = 0;
    }else if(window_type == "MIDDLE"){
      look_ahead = window / 2;
      look_back = window / 2 + (window % 2) - 1;
    }else if(window_type == "PAST"){
      look_ahead = 0;
      look_back  = 0;
    }

    inputsCache[inputID].lookAhead = look_ahead;
    inputsCache[inputID].lookBack = look_back;

    sum_count = 0;

    inOrder = true;

  }

  virtual void initialize()
  {

    int max_lookAhead = outputs[outputID].lookAhead; 
    int max_lookBack  = outputs[outputID].lookBack; 
    
    if(spectrumID != -1){ 
      max_lookAhead = max(max_lookAhead, outputs[spectrumID].lookAhead); 
      max_lookBack  = max(max_lookBack,  outputs[spectrumID].lookBack);       
    } 
    if(spectrumbinID != -1){ 
      max_lookAhead = max(max_lookAhead, outputs[spectrumbinID].lookAhead); 
      max_lookBack  = max(max_lookBack,  outputs[spectrumbinID].lookBack);       
    } 
    if(tfinfoID != -1){ 
      max_lookAhead = max(max_lookAhead, outputs[tfinfoID].lookAhead); 
      max_lookBack  = max(max_lookBack,  outputs[tfinfoID].lookBack);       
    } 
    if(rxxsID != -1){ 
      max_lookAhead = max(max_lookAhead, outputs[rxxsID].lookAhead); 
      max_lookBack  = max(max_lookBack,  outputs[rxxsID].lookBack);       
    } 
    if(rxxnID != -1){ 
      max_lookAhead = max(max_lookAhead, outputs[rxxnID].lookAhead); 
      max_lookBack  = max(max_lookBack,  outputs[rxxnID].lookBack);       
    } 
    
    outputs[outputID].lookAhead = 1 + max_lookAhead; 
    outputs[outputID].lookBack  = 1 + max_lookBack;     
    
    if(spectrumID != -1){ 
      outputs[spectrumID].lookAhead = 1 + max_lookAhead; 
      outputs[spectrumID].lookBack  = 1 + max_lookBack; 
    }
    if(spectrumbinID != -1){ 
      outputs[spectrumbinID].lookAhead = 1 + max_lookAhead; 
      outputs[spectrumbinID].lookBack  = 1 + max_lookBack; 
    }
    if(tfinfoID != -1){ 
      outputs[tfinfoID].lookAhead = 1 + max_lookAhead; 
      outputs[tfinfoID].lookBack  = 1 + max_lookBack; 
    }
    if(rxxsID != -1){ 
      outputs[rxxsID].lookAhead = 1 + max_lookAhead; 
      outputs[rxxsID].lookBack  = 1 + max_lookBack; 
    }
    if(rxxnID != -1){ 
      outputs[rxxnID].lookAhead = 1 + max_lookAhead; 
      outputs[rxxnID].lookBack  = 1 + max_lookBack; 
    }
    
    this->BufferedNode::initialize();
  }

  virtual int translateInput(string inputName) {
    if (inputName == "PARAMS") {
      paramsID = addInput(inputName);
      return paramsID;
    }
    else if (inputName == "TFNAME") {
      tfnameID = addInput(inputName);
      return tfnameID;
    }
    else if (inputName == "PARAMMAP") {
      parammapID = addInput(inputName);
      return parammapID;
    }
    else if (inputName == "OPERATION_FLAG") {
      operationID = addInput(inputName);
      return operationID;
    }
    else if (inputName == "NOISECM") {
      noisecmID = addInput(inputName);
      return noisecmID;
    }
    else if (inputName == "INPUT") {
      for (unsigned int i = 0; i < inputs.size(); i++) {
	if (inputs[i].name == inputName) {
	  return i;
	}
      }
    }
    else {throw new NodeException(this, inputName+ " is not supported.", __FILE__, __LINE__);}
  }

  // dynamic output-port translation
  virtual int translateOutput (string outputName)
  {

    if (outputName == "SPECTRUM") {
      if(bspectrumID){
	return spectrumID;
      }else{
	bspectrumID = true;
	return spectrumID = addOutput(outputName);
      }
    }

    if (outputName == "SPECTRUM_BIN") {
      if(bspectrumbinID){
	return spectrumbinID;
      }else{
	bspectrumbinID = true;
	return spectrumbinID = addOutput(outputName);
      }
    }

    if (outputName == "TF_INFO") {
      if(btfinfoID){
	return tfinfoID;
      }else{
	btfinfoID = true;
	return tfinfoID = addOutput(outputName);
      }
    }

    if (outputName == "RXXS") {
      if(brxxsID){
	return rxxsID;
      }else{
	brxxsID = true;
	return rxxsID = addOutput(outputName);
      }
    }

    if (outputName == "RXXN") {
      if(brxxnID){
	return rxxnID;
      }else{
	brxxnID = true;
	return rxxnID = addOutput(outputName);
      }
    }

    for (unsigned int i=0; i< outputNames.size(); i++) {	
      if (outputNames[i] == outputName) {
	return i;
      }
    }  
    
    return addOutput(outputName);
  }

  void calculate(int output_id, int count, Buffer &out)
  {
    
    // Load Noise Correlation Matrix

    if(music_algorithm != "SEVD"){

      if(noisecmID == -1){
	
	// Use identity matrix if NOISECM is not connected
	for(int k = 0 ; k < pslength ; k++){
	  for (int i = 0; i < nb_channels; i++) {
	    for (int j = 0; j < nb_channels; j++) {
	      if(i==j){
		RxxN[k](i, j) = complex<float>(1.0f,0.0f);
	      }else{
		RxxN[k](i, j) = complex<float>(0.0f,0.0f);
	      }
	    }
	  }
	}
	
      }
      else {
	
	RCPtr<FCMatrix> noise = getInput(noisecmID, count);
      
	int iNVecSrc = noise->nrows();  // num. of elements of frequency series (should be equal to pslength)
      
	if((noise->nrows() == 0)&&(RxxNinit_flag==0)){
	  // Use identity matrix if RxxN is not initialized
	  for(int k = 0 ; k < pslength ; k++){
	    for (int i = 0; i < nb_channels; i++) {
	      for (int j = 0; j < nb_channels; j++) {
		if(i==j){
		  RxxN[k](i, j) = complex<float>(1.0f,0.0f);
		}else{
		  RxxN[k](i, j) = complex<float>(0.0f,0.0f);
		}
	      }
	    }
	  }
	  RxxNinit_flag = 1;
	}else{
	  for(int k = 0 ; k < iNVecSrc ; k++){
	    for (int i = 0; i < nb_channels; i++) {
	      for (int j = 0; j < nb_channels; j++) {
		RxxN[k](i, j) = (*noise)(k, i*nb_channels + j);
	      }
	    }
	  }      
	}
      
      }

    }
        
    // Save data vector for spatial spectrum
    RCPtr<Vector<float > > spectrum(new Vector<float >(nh * nd * nr));    
    if(spectrumID != -1){    
      (*(outputs[spectrumID].buffer))[count] = spectrum;
      for (unsigned int i = 0; i < nh * nd * nr; i++) { 
	(*spectrum)[i] = 0.0f; 
      } 
    }

    // Save data vector for spatial spectrum
    RCPtr<Tensor> spectrumbin(new Tensor());
    if(spectrumbinID != -1){    
      (*(outputs[spectrumbinID].buffer))[count] = spectrumbin;
      (*spectrumbin).vsize.resize(4);
      (*spectrumbin).vsize[0] = pslength;
      (*spectrumbin).vsize[1] = nh;
      (*spectrumbin).vsize[2] = nd;
      (*spectrumbin).vsize[3] = nr;
      (*spectrumbin).data.resize(pslength * nh * nd * nr);
      for (unsigned int i = 0; i < (*spectrumbin).data.size(); i++) { 
	(*spectrumbin).data[i] = 0.0f; 
      }
    }

    if(tfinfoID != -1){    
      (*(outputs[tfinfoID].buffer))[count] = tf_info;
    }

    RCPtr<FCMatrix> rxxs(new FCMatrix(Rxx.size(), Rxx[0].size1()*Rxx[0].size2()));    
    if(rxxsID != -1){    
      (*(outputs[rxxsID].buffer))[count] = rxxs;
      for (int k = 0; k < Rxx.size(); k++) {
	for (int i = 0; i < Rxx[k].size1(); i++) {
	  for (int j = 0; j < Rxx[k].size2(); j++) {
	    (*rxxs)(k, i*Rxx[k].size2() + j) = 0.0f;
	  }
	}
      }   
    }

    RCPtr<FCMatrix> rxxn(new FCMatrix(RxxN.size(), RxxN[0].size1()*RxxN[0].size2()));    
    if(rxxnID != -1){    
      (*(outputs[rxxnID].buffer))[count] = rxxn;
      for (int k = 0; k < RxxN.size(); k++) {
	for (int i = 0; i < RxxN[k].size1(); i++) {
	  for (int j = 0; j < RxxN[k].size2(); j++) {
	    (*rxxn)(k, i*RxxN[k].size2() + j) = 0.0f;
	  }
	}
      }   
    }
    
    // Current Correlation Matrix Generation

    RCPtr<Matrix<complex<float> > > inputp;

    // Vector<ObjectRef>& sources = *new Vector<ObjectRef>;
    // (*(outputs[outputID].buffer))[count] = &sources;
    RCPtr<Vector<ObjectRef> > sources(Vector<ObjectRef>::alloc(0));
    (*(outputs[outputID].buffer))[count] = sources;

    if(operationID != -1){
      ObjectRef operation = getInput(operationID, count);
      int operation_flag;
      if (typeid(*operation) == typeid(Bool)) { 
	bool bInput = dereference_cast<bool>(operation); 
	operation_flag = (bInput ? 1 : 0); 
      }else if(typeid(*operation) == typeid(Int)){ 
	operation_flag = dereference_cast<int> (operation); 
      }else{ 
	throw new NodeException(this, string("The type of OPERATION_FLAG is not appropriate in LocalizeMUSIC."), __FILE__, __LINE__); 
      }       
      if(operation_flag == 0) return;
    }

    if(window_type == "PAST"){

      // inputCashe 
      inputp = getInput(inputID, count); 
      if (count == 0) { 
	if (!(channel_selection.size() == (*inputp).nrows())) { 
	  throw new NodeException(this, string("LocalizeMUSIC. Input channel size is not matched to the steering array."), __FILE__, __LINE__); 
	} 
      } 
      for (int chan = 0; chan < nb_channels; chan++) { 
	for (int k = 0; k < pslength; k++) { 
	  x(chan, k) = (*inputp)(chan, k); 
	} 
      } 
      if(xbuf.size() < window){ 
	xbuf.push_back(x); 
      }else{ 
	xbuf.push_back(x);       
	xbuf.pop_front();       
      } 
      
      if ((count % period == 0)&&((count != 0)||(period == 1))) {
	
	for (int i = 0; i < xbuf.size(); i++) { 
	  
	  for (int chan = 0; chan < nb_channels; chan++) {
	    for (int k = 0; k < pslength; k++) {
	      x(chan, k) = xbuf[i](chan,k);
	    }
	  }
	  
	  AddToCorrelation(Rxx, localizer->GetUsedFrequency(), x);
	  sum_count++;
	}
	NormalizeCorrelation(Rxx, localizer->GetUsedFrequency(), sum_count);
	sum_count = 0;
      }else {
	return;
      }
      
    }else{
      
      if (count % period == 0 && count >= look_back) { 
	for (int i = -look_back; i <= look_ahead; i++) { 
	  
	  inputp = getInput(inputID, count + i); 
	  
	  if (count == 0) { 
	    if (!(channel_selection.size() == (*inputp).nrows())) { 
	      throw new NodeException(this, string("LocalizeMUSIC. Input channel size is not matched to the steering array."), __FILE__, __LINE__); 
	    } 
	  } 
	  
	  for (int chan = 0; chan < nb_channels; chan++) {
	    for (int k = 0; k < pslength; k++) {
	      x(chan, k) = (*inputp)(chan, k);
	    }
	  }
	  
	  AddToCorrelation(Rxx, localizer->GetUsedFrequency(), x);
	  sum_count++;
	}
	NormalizeCorrelation(Rxx, localizer->GetUsedFrequency(), sum_count);
	sum_count = 0;
      }else {
	return;
      }
    }

#ifdef DEBUG
    ptime start_time;
    ptime end_time;
    start_time = microsec_clock::local_time();
#endif
    
    // MUSIC with GEVD
    
    localizer->Localize(Rxx, RxxN, mindeg, maxdeg, music_algorithm, MSSW, enable_eigenvalue_weight, enable_hierarchical);
    
    int n_result = localizer->GetFoundSourceCount();

    for (int i = 0; i < n_result; i++) {
      if (is_debug) {
	if(!enable_hierarchical) 
	  printf("%d: %7.2f %7.2f %7.2f %7.2f\t", i,
		 (float)(localizer->GetSearchHeight()[localizer->GetFoundHeight()[i]]),
		 (float)(localizer->GetSearchDirection()[localizer->GetFoundDirection()[i]]),
		 (float)(localizer->GetSearchRange()[localizer->GetFoundRange()[i]]),
		 localizer->GetP(localizer->GetFoundHeight()[i],localizer->GetFoundDirection()[i],localizer->GetFoundRange()[i]));
	else 
	  printf("%d: %7.2f %7.2f %7.2f %7.2f\t", i,
		 (float)(localizer->GetSearchHeight_Itpl()[localizer->GetFoundHeight_Itpl()[i]]),
		 (float)(localizer->GetSearchDirection_Itpl()[localizer->GetFoundDirection_Itpl()[i]]),
		 (float)(localizer->GetSearchRange_Itpl()[localizer->GetFoundRange_Itpl()[i]]),
		 localizer->GetFoundPower_Itpl()[i]);
	// localizer->GetP_Itpl(localizer->GetFoundHeight_Itpl()[i],localizer->GetFoundDirection_Itpl()[i],localizer->GetFoundRange_Itpl()[i]));
      }
                
      RCPtr<Source> src(new Source);
            
      if(!enable_hierarchical){ 
	const int found_height    = localizer->GetFoundHeight()[i];
	const int found_direction = localizer->GetFoundDirection()[i];
	const int found_range     = localizer->GetFoundRange()[i];
	src->id = 0;
	src->power = localizer->GetP(found_height,found_direction,found_range);
	float theta = localizer->GetSearchDirection()[found_direction] * M_PI / 180.0f;
	float r = localizer->GetSearchRange()[found_range];
	float phi = localizer->GetSearchHeight()[found_height] * M_PI / 180.0f;
	src->x[0] = r * cos(theta) * cos(phi);
	src->x[1] = r * sin(theta) * cos(phi);
	src->x[2] = r * sin(phi);
	src->tfindex[Source::TFHeight] = found_height;
	src->tfindex[Source::TFDirection] = found_direction;
	src->tfindex[Source::TFRange] = found_range;      
	src->tfindex_max[Source::TFHeight] = localizer->GetSearchHeight().size() - 1;
	src->tfindex_max[Source::TFDirection] = localizer->GetSearchDirection().size() - 1;
	src->tfindex_max[Source::TFRange] = localizer->GetSearchRange().size() - 1;      
	src->tfindex_min[Source::TFHeight] = 0;
	src->tfindex_min[Source::TFDirection] = 0;
	src->tfindex_min[Source::TFRange] = 0;      
	src->compare_mode = Source::TFINDEX;
      }else{ 
	const int found_height    = localizer->GetFoundHeight_Itpl()[i];
	const int found_direction = localizer->GetFoundDirection_Itpl()[i];
	const int found_range     = localizer->GetFoundRange_Itpl()[i];
	src->id = 0;
	// src->power = localizer->GetP_Itpl(found_height,found_direction,found_range);
	src->power = localizer->GetFoundPower_Itpl()[i];
	float theta = localizer->GetSearchDirection_Itpl()[found_direction] * M_PI / 180.0f;
	float r = localizer->GetSearchRange_Itpl()[found_range];
	float phi = localizer->GetSearchHeight_Itpl()[found_height] * M_PI / 180.0f;
	src->x[0] = r * cos(theta) * cos(phi);
	src->x[1] = r * sin(theta) * cos(phi);
	src->x[2] = r * sin(phi);
	src->tfindex[Source::TFHeight] = found_height;
	src->tfindex[Source::TFDirection] = found_direction;
	src->tfindex[Source::TFRange] = found_range;      
	src->tfindex_max[Source::TFHeight] = localizer->GetSearchHeight().size() - 1;
	src->tfindex_max[Source::TFDirection] = localizer->GetSearchDirection().size() - 1;
	src->tfindex_max[Source::TFRange] = localizer->GetSearchRange().size() - 1;      
	src->tfindex_min[Source::TFHeight] = 0;
	src->tfindex_min[Source::TFDirection] = 0;
	src->tfindex_min[Source::TFRange] = 0;      
	src->compare_mode = Source::TFINDEX;
      }
      sources->push_back(src);
      
    }

    if (is_debug) {
      printf("\n");
      printf("MUSIC spectrum: ");
      for (int i = 0; i < nh; i++) {
	for (int j = 0; j < nd; j++) {
	  for (int k = 0; k < nr; k++) {
	    printf("%f ", localizer->GetP(i,j,k));
	  }
	}
      }
      printf("\n");
    }

    // Output the spacial spectrum
    if(spectrumID != -1){
      for (int i = 0; i < nh; i++) {
	for (int j = 0; j < nd; j++) {
	  for (int k = 0; k < nr; k++) {
	    (*spectrum)[k + j * nr + i * nr * nd] = localizer->GetP(i,j,k);
	  }
	}
      }    
    }

    // Output the spacial spectrum of each bin
    if(spectrumbinID != -1){
      for (int bin = 0; bin < pslength; bin++) {
	for (int ih = 0; ih < nh; ih++) {
	  for (int id = 0; id < nd; id++) {
	    for (int ir = 0; ir < nr; ir++) {
	      (*spectrumbin).data[ir + id * nr + ih * nr * nd + bin * nr * nd * nh] = localizer->GetP_bin(bin,ih,id,ir);
	    }
	  }
	}    
      }
    }

    if(rxxsID != -1){    
      for (int k = 0; k < Rxx.size(); k++) {
	for (int i = 0; i < Rxx[k].size1(); i++) {
	  for (int j = 0; j < Rxx[k].size2(); j++) {
	    (*rxxs)(k, i*Rxx[k].size2() + j) = Rxx[k](i, j);
	  }
	}
      }   
    }

    if(rxxnID != -1){    
      for (int k = 0; k < RxxN.size(); k++) {
	for (int i = 0; i < RxxN[k].size1(); i++) {
	  for (int j = 0; j < RxxN[k].size2(); j++) {
	    (*rxxn)(k, i*RxxN[k].size2() + j) = RxxN[k](i, j);
	  }
	}
      }   
    }

    //cout << sources.size() << endl;

    ClearCorrelation(Rxx, localizer->GetUsedFrequency());
    ClearCorrelation(RxxN, localizer->GetUsedFrequency());

#ifdef DEBUG
      end_time = microsec_clock::local_time();
      cout << end_time - start_time << endl;
#endif

    if((paramsID != -1)&&(parammapID == -1)){
      RCPtr<Vector<ObjectRef> > param_ptr = getInput(paramsID, count);
      const Vector<ObjectRef>& param_vec = *param_ptr;      
      if(param_vec.size() != 0){	
	RCPtr<ParamsLocalizeMUSIC> value_LocalizeMUSIC = param_vec[0];
	num_source = (*value_LocalizeMUSIC).num_source;
	localizer->SetNumSource(num_source);
	mindeg = (*value_LocalizeMUSIC).min_deg;
	maxdeg = (*value_LocalizeMUSIC).max_deg;
	lower_bound = (*value_LocalizeMUSIC).lower_bound_frequency;
	upper_bound = (*value_LocalizeMUSIC).upper_bound_frequency;
	localizer->SetUsedFrequency(lower_bound, upper_bound);
      }
    }
    if(tfnameID != -1){
      ObjectRef tfnameValue = getInput(tfnameID, count);
      const String &tfname = object_cast<String> (tfnameValue);
      if(ptfname != tfname){
	localizer->ReadTransferFunction(const_cast<char *>(tfname.c_str()), ch_selection, enable_interpolation, enable_hierarchical, interpolation_type, resolution);
	nh = localizer->GetSearchHeight().size();
	nd = localizer->GetSearchDirection().size();
	nr = localizer->GetSearchRange().size();    
      }
      ptfname = tfname;
    }
    if(parammapID != -1){      
      ObjectRef p_input = getInput(parammapID, count);
      const Map<string, ObjectRef>& p_input_map =
	object_cast<Map<string, ObjectRef> >(p_input);    
      Map<string, ObjectRef>::const_iterator it;
      for (it = p_input_map.begin(); it != p_input_map.end(); ++it) {
	if(it->first != "LocalizeMUSIC") continue;
	const Map<string, ObjectRef>& map_contents =
	  object_cast<Map<string, ObjectRef> >(it->second);
	Map<string, ObjectRef>::const_iterator it_sub;
	for (it_sub = map_contents.begin(); it_sub != map_contents.end(); ++it_sub) {
	  if(it_sub->first == "NUM_SOURCE"){
	    if(typeid(*it_sub->second) == typeid(Int)){		
	      num_source = dereference_cast<int>(it_sub->second);
	      localizer->SetNumSource(num_source);
	    }
	  }
	  if(it_sub->first == "MIN_DEG")
	    if(typeid(*it_sub->second) == typeid(Int))
	      mindeg = dereference_cast<int>(it_sub->second);
	  if(it_sub->first == "MAX_DEG")
	    if(typeid(*it_sub->second) == typeid(Int))
	      maxdeg = dereference_cast<int>(it_sub->second);
	  if(it_sub->first == "LOWER_BOUND_FREQUENCY")
	    if(typeid(*it_sub->second) == typeid(Int))
	      lower_bound = dereference_cast<int>(it_sub->second);
	  if(it_sub->first == "UPPER_BOUND_FREQUENCY")
	    if(typeid(*it_sub->second) == typeid(Int))
	      upper_bound = dereference_cast<int>(it_sub->second);
	  localizer->SetUsedFrequency(lower_bound, upper_bound);
	  if(it_sub->first == "A_MATRIX"){
	    if(typeid(*it_sub->second) == typeid(String)){
	      string tfname = object_cast<String>(it_sub->second);
	      localizer->ReadTransferFunction(const_cast<char *>(tfname.c_str()), ch_selection, enable_interpolation, enable_hierarchical, interpolation_type, resolution);
	      nh = localizer->GetSearchHeight().size();
	      nd = localizer->GetSearchDirection().size();
	      nr = localizer->GetSearchRange().size();    
	    }
	  }
	}
      }
    }

  }

  //IN_ORDER_NODE_SPEEDUP(LocalizeMUSIC);
};

#endif
#endif
