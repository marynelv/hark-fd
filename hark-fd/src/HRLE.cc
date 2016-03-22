// ---------------------------------------------------------------------
/**
 Copyright (c) 2008 Honda Research Institute Japan, Co., Ltd.

 \file HRLE.cc
 \brief	\~English It executes HRLE.
 \brief	\~Japanese HRLE$B$N<B9T(B
 \author	Hirofumi Nakajima
 \date	5 Aug. 2009
 \version	$Id: HRLE.cc  $
*/
// ---------------------------------------------------------------------
//#include <complex.h>
#include <stdlib.h>
#include "BufferedNode.h" 
#include "Map.h" // map+Object
#include "Vector.h" // vector+BaseVector(Object) 
//#include <complex>
//#include <algorithm>
//#include "Matrix.h"
//#include "Source.h"
// ---------------------------------------------------------------------
//#include "MICARYHA_struct.h"
//#include "MICARY_struct.h"
//#include "MICARY_PF_Struct.h"
//#include "ArrayInit.h"
//#include "PF_Init.h"
#include "lppostv1forHARK.cpp"
#include "HarkParamsObjects.h"

// ---------------------------------------------------------------------

// namespace$B;HMQ$r@k8@$9$k$3$H(B
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class HRLE; // $B"+%N!<%I(B

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// $B%N!<%I$NF~=PNOC<;R$r:n@.$9$k(B
DECLARE_NODE(HRLE) // $B"+%N!<%I(B

/*Node
 *  
 * @name HRLE
 * @category HARK:Separation
 * @description This executes HRLE.
 *
 * @input_name INPUT_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Input spectrum.The key is a source ID,and the value is state Input power spectrum(Vector<float>)
 *
 * @output_name NOISE_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Estimated Noise spectrum.The key is a source ID,and the value is the noise spectrum(Vector<float>)
 *   
 * @parameter_name LX
 * @parameter_type float
 * @parameter_value 0.85
 * @parameter_description Lx value of estimation, e.g. Lx=0 -> Minimum (MCRA), Lx=0.5 -> Median , Lx=1.0 -> Maximum [default:0.85]
 *
 * @parameter_name TIME_CONST_METHOD
 * @parameter_type string
 * @parameter_value LEGACY
 * @parameter_list LEGACY:MILLISECOND
 * @parameter_description Time constant value definition, "LEGACY" uses time constant value for HARK 2.0.0,"MILLISECOND" uses time constant value in frames. [default: LEGACY]
 * 
 * @parameter_name TIME_CONSTANT
 * @parameter_type float
 * @parameter_value 
 * @parameter_valid TIME_CONST_METHOD=LEGACY
 * @parameter_description Time constant for exponential decay window in samples [default:]
 * 
 * @parameter_name DECAY_FACTOR
 * @parameter_type int
 * @parameter_value 1000
 * @parameter_valid TIME_CONST_METHOD=MILLISECOND
 * @parameter_description Time constant for exponential decay window in millisecond [default:100]
 *
 * @parameter_name ADVANCE
 * @parameter_type int
 * @parameter_value 160
 * @parameter_valid TIME_CONST_METHOD=MILLISECOND
 * @parameter_description The length in sample between a frame and a previous frame. [default: 160]
 *
 * @parameter_name SAMPLING_RATE
 * @parameter_type int
 * @parameter_value 16000
 * @parameter_valid TIME_CONST_METHOD=MILLISECOND
 * @parameter_description Sampling rate (Hz) [default: 16000].
 *
 * @parameter_name NUM_BIN
 * @parameter_type float
 * @parameter_value 1000
 * @parameter_description Number of histogram bins [default:1000]
 * 
 * @parameter_name MIN_LEVEL
 * @parameter_type float
 * @parameter_value -100
 * @parameter_description Minimum level of histogram bin in dB [default:-100]
 * 
 * @parameter_name STEP_LEVEL
 * @parameter_type float
 * @parameter_value 0.2
 * @parameter_description Step level of histogram bin (Width of each histogram bin) in dB [default:0.2]
 *
 * @parameter_name DEBUG
 * @parameter_type bool
 * @parameter_value false
 * @parameter_list true:false
 * @parameter_description Prints the histogram for each 100 iterations.

END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class HRLE : public BufferedNode 
{

  int m_iInputSpecID; // $BF~NO%9%Z%/%H%k(BID
  int paramsID;
  int parammapID;
  int resetID;
  int m_iNoiseSpecID; // $B=PNO%9%Z%/%H%k(BID
  HPostv4 PF; // postfilter class (lppost1forHARK.cpp)
  bool isDebug;
public:
    // $B%3%s%9%H%i%/%?!J=i4|2=!K(B
	HRLE(string nodeName, ParameterSet params)
	  : BufferedNode(nodeName, params),
	    paramsID(-1), parammapID(-1), resetID(-1)
	{ 
        inOrder = true; // BufferedNode$B$N%a%s%P(B

        // $BF~=PNO(BID$B$N<hF@(B
		m_iInputSpecID = addInput("INPUT_SPEC");
		m_iNoiseSpecID = addOutput("NOISE_SPEC");		
		
		//$B%Q%i%a!<%?$N<hF@(B
//		if (parameters.exist("Lx")){}else{} $B$H$9$k$H6uGr$N;~$N=hM}$,$G$-$k(B

	if (parameters.exist("TIME_CONST_METHOD")){
		string sTimeConst;
		sTimeConst = object_cast<string> (parameters.get("TIME_CONST_METHOD"));		
		if (sTimeConst == "LEGACY"){
			PF.tr   = 1.0 - 1.0 / ((dereference_cast<float>(parameters.get("TIME_CONSTANT")))*60.0); //orginal
		}
		else if (sTimeConst == "MILLISECOND"){
			int msec = dereference_cast<int>(parameters.get("DECAY_FACTOR"));
			int advance = dereference_cast<int>(parameters.get("ADVANCE"));
			int samples = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
			int frames = msec * samples / (1000.0 * advance);
			
			PF.tr   = 1.0 - 1.0 / frames; 
		}
	}					
	else{
		if  (parameters.exist("TIME_CONSTANT")){
			PF.tr   = 1.0 - 1.0 / ((dereference_cast<float>(parameters.get("TIME_CONSTANT")))*60); //orginal
   			//PF.tr_inv = 1.0 / PF.tr;			
		}
	}	
	
        PF.Lx   = dereference_cast<float>(parameters.get("LX"));
        PF.N    = dereference_cast<float>(parameters.get("NUM_BIN"));
        PF.Min  = dereference_cast<float>(parameters.get("MIN_LEVEL"));
        PF.Step = dereference_cast<float>(parameters.get("STEP_LEVEL"));
	isDebug = dereference_cast<bool>(parameters.get("DEBUG"));
	} 

	~HRLE(){} // some descriptions e.g. Free Memory

  virtual int translateInput(string inputName) {
    if (inputName == "PARAMS") {
      paramsID = addInput(inputName);
      return paramsID;
    }
    else if (inputName == "PARAMMAP") {
      parammapID = addInput(inputName);
      return parammapID;
    }
    else if (inputName == "RESET_HIST") {
      resetID = addInput(inputName);
      return resetID;
    }
    else if (inputName == "INPUT_SPEC") {
      for (unsigned int i = 0; i < inputs.size(); i++) {
	if (inputs[i].name == inputName) {
	  return i;
	}
      }
    }
    else {throw new NodeException(this, inputName+ " is not supported.", __FILE__, __LINE__);}
  }

  // $B7W;;%3%"(B
  void calculate(int output_id, int count, Buffer &out)
  {
		int iNFreq, iNSource;
		int nsrc, nfrq;
        Compmat X, X1, Y;
        vector<int> viID;
        vector<float> vftmp;

		//$BF~NO!"=PNO$N%P%C%U%!$r:n@.(B vector<int>$B7?(B 
		map<int,vector<float> > mInSpec; //$BF~NO(B
		map<int,vector<float> > mOutNoiseSpec; //$B=PNO(B


        RCPtr<Map<int,ObjectRef> > orInputSpecTemp(getInput(m_iInputSpecID, count)); // $B<BBN(B(Map)$B$r;X$9%]%$%s%?(B

	if((paramsID != -1)&&(parammapID == -1)){
	  RCPtr<Vector<ObjectRef> > param_ptr = getInput(paramsID, count);
	  const Vector<ObjectRef>& param_vec = *param_ptr;
	  if(param_vec.size() != 0){
	    RCPtr<ParamsHRLE> value_HRLE = param_vec[0];
	    PF.Lx = (*value_HRLE).lx;
	    PF.tr = 1.0 - 1.0 / ((*value_HRLE).time_constant * 60.0);
	  }
	}

	if(parammapID != -1){      
	  ObjectRef p_input = getInput(parammapID, count);
	  const Map<string, ObjectRef>& p_input_map =
	    object_cast<Map<string, ObjectRef> >(p_input);    
	  Map<string, ObjectRef>::const_iterator it;
	  for (it = p_input_map.begin(); it != p_input_map.end(); ++it) {
	    if(it->first != "HRLE") continue;
	    const Map<string, ObjectRef>& map_contents =
	      object_cast<Map<string, ObjectRef> >(it->second);
	    Map<string, ObjectRef>::const_iterator it_sub;
	    for (it_sub = map_contents.begin(); it_sub != map_contents.end(); ++it_sub) {
	      if(it_sub->first == "LX")
		if(typeid(*it_sub->second) == typeid(Float))		
		  PF.Lx = dereference_cast<float>(it_sub->second);
	      if(it_sub->first == "TIME_CONSTANT")
		if(typeid(*it_sub->second) == typeid(Float))
		  PF.tr = 1.0 - 1.0 / (dereference_cast<float>(it_sub->second) * 60.0);
	      if(it_sub->first == "DECAY_FACTOR"){
		if(typeid(*it_sub->second) == typeid(Int)){
		  int msec = dereference_cast<int>(it_sub->second);
		  int advance = dereference_cast<int>(parameters.get("ADVANCE"));
		  int samples = dereference_cast<int>(parameters.get("SAMPLING_RATE"));
		  int frames = msec * samples / (1000.0 * advance);
		  PF.tr   = 1.0 - 1.0 / frames; 
		}
	      }
	    }
	  }
	}

	if(resetID != -1){
		RCPtr<Bool > orResetHistTemp(getInput(resetID, count));
		if (*(orResetHistTemp) == true){
			PF.resetHist();
			cout<<"----- HRLE : Reset Histogram -----"<< endl;
		}
	}

		// Output$B$N=`Hw(B
        Buffer &NoiseSpecBuffer = *(outputs[m_iNoiseSpecID].buffer) ; // outputs[*ID].buffer$B$O(BFD$B$N=PNOMQJQ?t(B, ID$B$O%3%s%9%H%i%/%?$G@_Dj(B
        ObjectRef orNoiseSpecTemp(new Map<int,ObjectRef>) ; // $B6u$N(BObjectRef$B7?JQ?t!"=PNO$N3JG<MQ(B
        NoiseSpecBuffer[count] = orNoiseSpecTemp; // $B8=:_$N;~4V%+%&%s%H(Bcount$B$X!"=PNOJQ?t$X$N%]%$%s%?$rF~$l$k$N$,7h$^$j(B
        Map<int,ObjectRef> &mNoiseSpecvalue = object_cast<Map<int,ObjectRef> >(orNoiseSpecTemp); //$BL>A0$NIU$1D>$7$H7?JQ49(B(Map$B7?(B)

     
		Map<int,ObjectRef>::iterator       itTempOR; // Map$BMQ(B(FD$B$+$i$NF~NO(B)
        itTempOR = orInputSpecTemp->begin(); // FD$B$+$i$NF~NO%G!<%?(B(Map$B7?(B)$B$N%$%F%l!<%?(B($B=i$a$N%Z%"$r;X<((B)
        iNSource = orInputSpecTemp->size() ; //$B2;8;?t(B($B%Z%"?t(B)
		
		if (iNSource>0)
		{
			
			vector<float> &vfTemp = object_cast<Vector<float> >(itTempOR->second); // $B=i$a$N2;8;$NCf?H!J%9%Z%/%H%k!K(B
			iNFreq   = vfTemp.size() ;	//$B!!<~GH?t%S%s?t(B
			//cout << "#Freqnency bin = " << iNFreq << endl;


			X.zeros(iNSource, iNFreq); // $BF~NO%G!<%?NN0h3NJ](B
			viID.reserve(iNSource); // ID$BJ]B8MQNN0h3NJ](B
			for(nsrc = 0; itTempOR != orInputSpecTemp->end() ; itTempOR++, nsrc++){
				viID[nsrc]                   = itTempOR->first;
				vector<float> &vcfInputSpec = object_cast<Vector<float> >(itTempOR->second);
				//printf("Input[0]=%f ",vcfInputSpec[0]);
				for(nfrq = 0; nfrq < iNFreq; nfrq++){
					X(nsrc, nfrq) = vcfInputSpec[nfrq];
				}
			}
	        
			PF.tr_inv = 1.0 / PF.tr / iNSource;
			// postfilter$B$N<B9T!J3F2;8;$K$D$$$F!"FbIt?dDj;(2;%R%9%H%0%i%`$O6&DL$KMxMQ!K(B
			// $B!\=PNOMQJQ?t(BmNoiseSpecvalue( map<int, vector<float> >$B7?(B) $B$N3FMWAG$K%G!<%?$r5M$a$k(B
			for(nsrc = 0 ; nsrc < iNSource; nsrc++){ // $B2;8;$N%k!<%W(B

				ObjectRef         orOutTmp = ObjectRef(Vector<float>::alloc(iNFreq)) ; // $B=PNOMQJQ?t!J#12;8;J,!K(B, ObjectRef(Vector<float>)$B7?(B, $B6u(B
				vector<float> & vcfOutTmp = object_cast<vector<float> >(orOutTmp) ; // $B7?$r(Bvector<float>$B$KL>A0$O(BvcfOutputSpec, $B6u(B

				X1 = X.mid1(nsrc);
				Y = PF.execute(X1); // Y$B$K$O3F2;8;$G$N%N%$%:?dDjCM$,F~$k(B
				
				for(nfrq = 0; nfrq < iNFreq; nfrq++){
					vcfOutTmp[nfrq] = Y.r(nfrq); // vector<float>$B$K$D$a$k(B
				
				}
				mNoiseSpecvalue.insert(make_pair(viID[nsrc], orOutTmp)) ; // $B=PNOJQ?t$X$N<BBN$N3JG<(B
			}	
		}

		if(isDebug && count % 100 == 0){
		  PF.disp();
		}
	}

};
