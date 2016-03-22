// ---------------------------------------------------------------------
/**
 Copyright (c) 2010 Honda Research Institute Japan, Co., Ltd.

 \file SpectralGainFilter.cc
 \brief	\~English It estimates the voice spectra.
 \brief	\~Japanese 音声を推定する
 \author	Kenichi Morio
 \date	28 Aug 2008
 \version	$Id: SpectralGainFilter.cc 450 2009-03-16 10:12:38Z suzuki $
*/
// ---------------------------------------------------------------------

#include <algorithm>
#include "BufferedNode.h" 
#include "Map.h"
#include "Vector.h"
// ---------------------------------------------------------------------
#include "MICARYHA_struct.h"
#include "MICARY_struct.h"
#include "MICARY_PF_Struct.h"
// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

//forward declaration of class VAdd for use with the DECLARE_NODE macro
class SpectralGainFilter; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(SpectralGainFilter) // ←ノード

/*Node
 *  
 * @name SpectralGainFilter
 * @category HARK:Separation
 * @description This estimates the voice spectrum.
 *
 * @input_name INPUT_SPEC
 * @input_type Map<int,ObjectRef>
 * @input_description Separated spectrum(Vector<complex<float> >) with a key(source ID).
 *
 * @input_name VOICE_PROB
 * @input_type Map<int,ObjectRef>
 * @input_description Probability of voice existence(Vector<float>) with a key(source ID).
 *
 * @input_name GAIN
 * @input_type Map<int,ObjectRef>
 * @input_description Optimum gain coefficients(Vector<float>) with a key(source ID).
 *
 * @output_name OUTPUT_SPEC
 * @output_type Map<int,ObjectRef>
 * @output_description Estimated voice spectrum(Vector<complex<float> >) with a key(source ID).
 * 
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class SpectralGainFilter : public BufferedNode {

	int m_iInputSpecID;
	int m_iVoicePID;
	int m_iGainID;
	int m_iOutputSpecID;

public:
	SpectralGainFilter(string nodeName, ParameterSet params)
		: BufferedNode(nodeName, params)
	{ 
		//In the constructor, we create both the inputs and outputs.
		m_iInputSpecID = addInput("INPUT_SPEC");
		m_iVoicePID = addInput("VOICE_PROB");
		m_iGainID = addInput("GAIN");
		m_iOutputSpecID = addOutput("OUTPUT_SPEC");
	} 

	~SpectralGainFilter()
	{ 
//
	}

	//This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {
		// InputされるMapにVoiceP, Gainなどの係数をかけ結果を出力する
		int i, k;
		int iNCh;
		Map<int, ObjectRef>::iterator	pmapInputSpec, pmapFindTemp;

		// Input
		ObjectRef orInputSpecTemp = getInput(m_iInputSpecID, count);
		Map<int, ObjectRef> &mapInputSpec = object_cast<Map<int, ObjectRef> >(orInputSpecTemp);

		ObjectRef orVoicePTemp = getInput(m_iVoicePID, count);
		Map<int, ObjectRef> &mapVoiceP = object_cast<Map<int, ObjectRef> >(orVoicePTemp);

		ObjectRef orGainTemp = getInput(m_iGainID, count);
		Map<int, ObjectRef> &mapGain = object_cast<Map<int, ObjectRef> >(orGainTemp);

		// Output
		ObjectRef orOutputSpecTemp = ObjectRef(new Map<int, ObjectRef>());
		Map<int, ObjectRef> &mapOutputSpec = object_cast<Map<int, ObjectRef> >(orOutputSpecTemp);
		out[count] = orOutputSpecTemp;

		iNCh = mapInputSpec.size();
		if (iNCh == 0){
			return;
		}

		// 推定を行う
		pmapInputSpec = mapInputSpec.begin();
		for (i = 0 ; i < iNCh ; i++){
			// 2008/09/16 changed by N.Tanaka
			/*
			try {
				// 入力値の確認
				Vector<complex<float> > &vcfInputSpec = object_cast<Vector<complex<float> > >(pmapInputSpec->second);
				Vector<float> &vfVoiceP = object_cast<Vector<float> >(mapVoiceP[pmapInputSpec->first]);
				Vector<float> &vfGain = object_cast<Vector<float> >(mapGain[pmapInputSpec->first]);

				if (vfVoiceP.size() != vfGain.size()){
					throw (new NodeException(NULL, "Exception is caught in SpectralGainFilter calcutate", __FILE__, __LINE__));
				}

				// 出力用の箱を準備
				ObjectRef orOutputSpecTemp = ObjectRef(new Vector<complex<float> >);
				Vector<complex<float> >	&vcfOutputSpec = object_cast<Vector<complex<float> > >(orOutputSpecTemp);

				vcfOutputSpec.resize(vcfInputSpec.size());

				// 倍率をかける
				for (k = 0 ; k < vcfInputSpec.size() ; k++){
					if (k < vfVoiceP.size()){
						vcfOutputSpec[k] = complex<float>(vcfInputSpec[k].real()*vfVoiceP[k]*vfGain[k],
																			 vcfInputSpec[k].imag()*vfVoiceP[k]*vfGain[k]);
					}
					else {
						vcfOutputSpec[k] = vcfInputSpec[k];
					}
				}

				// 音源ID, スペクトルの出力
				mapOutputSpec.insert(pair<int, ObjectRef>(pmapInputSpec->first, orOutputSpecTemp));

				pmapInputSpec++;

			}
			catch (BaseException *e){
				throw (new NodeException(NULL, "Exception is caught in SpectralGainFilter calcutate", __FILE__, __LINE__));
			}
			*/
			// 入力値の確認
			Vector<complex<float> > &vcfInputSpec = object_cast<Vector<complex<float> > >(pmapInputSpec->second);
			pmapFindTemp = mapVoiceP.find(pmapInputSpec->first);
			if (pmapFindTemp == mapVoiceP.end()){
				throw (new NodeException(NULL, "Exception is caught in SpectralGainFilter calcutate(Failed to find ID in VOICE_P)", __FILE__, __LINE__));
			}
			pmapFindTemp = mapGain.find(pmapInputSpec->first);
			if (pmapFindTemp == mapGain.end()){
				throw (new NodeException(NULL, "Exception is caught in SpectralGainFilter calcutate(Failed to find ID in GAIN)", __FILE__, __LINE__));
			}
			Vector<float> &vfVoiceP = object_cast<Vector<float> >(mapVoiceP[pmapInputSpec->first]);
			Vector<float> &vfGain = object_cast<Vector<float> >(mapGain[pmapInputSpec->first]);

			if (vfVoiceP.size() != vfGain.size()){
				throw (new NodeException(NULL, "Exception is caught in SpectralGainFilter calcutate", __FILE__, __LINE__));
			}

			// 出力用の箱を準備
			ObjectRef orOutputSpecTemp = ObjectRef(new Vector<complex<float> >);
			Vector<complex<float> >	&vcfOutputSpec = object_cast<Vector<complex<float> > >(orOutputSpecTemp);

				vcfOutputSpec.resize(vcfInputSpec.size());

				// 倍率をかける
				for (k = 0 ; k < vcfInputSpec.size() ; k++){
					if (k < vfVoiceP.size()){
						vcfOutputSpec[k] = complex<float>(vcfInputSpec[k].real()*vfVoiceP[k]*vfGain[k],
																			 vcfInputSpec[k].imag()*vfVoiceP[k]*vfGain[k]);
					}
					else {
						vcfOutputSpec[k] = vcfInputSpec[k];
					}
				}

				// 音源ID, スペクトルの出力
				mapOutputSpec.insert(pair<int, ObjectRef>(pmapInputSpec->first, orOutputSpecTemp));

				pmapInputSpec++;
		}

  }

};
