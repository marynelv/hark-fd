// ---------------------------------------------------------------------
/*
 * Copyright 2009 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
// ---------------------------------------------------------------------
#include <iostream>
#include "BufferedNode.h"
#include "Buffer.h" 
//#include "Matrix.h"
#include "Map.h"
#include "Vector.h"
#include <complex>

// ---------------------------------------------------------------------

// namespace使用を宣言すること
using namespace FD;
using namespace std;

class SourceSelectorByID; // ←ノード

//Declaration of the node. This definition is transformed into XML data for the GUI, as well as documentation for the node
// ノードの入出力端子を作成する
DECLARE_NODE(SourceSelectorByID) // ←ノード

/*Node
 *  
 * @name SourceSelectorByID
 * @category HARK:MISC
 * @description Select sources using source ID.
 *
 * @input_name INPUT
 * @input_type Map<int,ObjectRef>
 * @input_description Power spectrum. The key is source ID, and the value is a (power) spectrum.
 *
 * @output_name OUTPUT
 * @output_type Map<int,ObjectRef>
 * @output_description Power spectrum of selected sources.
 * 
 * @parameter_name MIN_ID
 * @parameter_type int
 * @parameter_value 0
 * @parameter_description Minimum ID for source selection. Any source whose ID is upper than MIN_ID is selected. 
END*/

// ---------------------------------------------------------------------

//Class definition/implementation. Note that because we won't need to derive from this class, we don't need a header file (.h) and we can put everything in the .cc. Our node, like most other nodes, derives from BufferedNode.

class SourceSelectorByID : public BufferedNode {

  int m_iInputID;
  int m_iOutputID;
  int min_id;

public:
  SourceSelectorByID(string nodeName, ParameterSet params)
    : BufferedNode(nodeName, params)
  { 

    //In the constructor, we create both the inputs and outputs.
         m_iInputID = addInput("INPUT");
	 m_iOutputID = addOutput("OUTPUT");
	 min_id = dereference_cast<int>(parameters.get("MIN_ID"));

     inOrder = true;
  } 


    //This is the main method for the node, it is called from the BufferedNode class each time a result needs to be calculated.
  void calculate(int output_id, int count, Buffer &out)
  {
    
    RCPtr<Map<int, ObjectRef> > input = getInput(m_iInputID, count) ;
    RCPtr<Map<int, ObjectRef> > output(new Map<int, ObjectRef>) ;

    Map<int, ObjectRef>::iterator it = input->begin() ;
    for(;it != input->end(); it++){
      
      if(it->first >= min_id){
	
	if(typeid(*(it->second)) == typeid(Vector<float> )){
	  RCPtr<Vector<float> > in_power = it->second ;
	  RCPtr<Vector<float> > out_power(new Vector<float>) ;
	  out_power->reserve(in_power->size()) ;
	  
	  for(int i = 0 ; i < in_power->size() ; i++){
	    out_power->push_back((*in_power)[i]);
	  }
	  (*output)[it->first] =out_power;

	}
	else if(typeid(*(it->second)) == typeid(Vector<complex<float> >)){
	  RCPtr<Vector<complex<float > > > in_power = it->second ;
	  RCPtr<Vector<complex<float > > > out_power(new Vector<complex<float > >) ;
	  out_power->reserve(in_power->size()) ;

	  for(int i = 0 ; i < in_power->size() ; i++){
	    out_power->push_back((*in_power)[i]) ;
	  }

	  (*output)[it->first] = out_power ;
        }
    }
   }
   out[count] = output ;
  }

  IN_ORDER_NODE_SPEEDUP(SourceSelectorByID);
};
