/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#ifndef __TENSOR_H__
#define __TENSOR_H__

#include "Object.h"
#include "ObjectParser.h"
#include "misc.h"
#include <cmath>

namespace FD {

class Tensor : public Object {
  public:
    std::vector<int> vsize;
    std::vector<float> data;

    Tensor() {
    }
    
    Tensor(const Tensor& input) {
      vsize.resize(input.vsize.size());
      for(int cnt = 0; cnt < input.vsize.size(); cnt++){
	vsize[cnt] = input.vsize[cnt];
      }
      data.resize(input.data.size());
      for(int cnt = 0; cnt < input.data.size(); cnt++){
	data[cnt] = input.data[cnt];
      }
    }

    ~Tensor() {
    }

    bool iscomplex()
    {
      int dim = data.size();
      for(int cnt = 0; cnt < vsize.size(); cnt++){
	dim /= vsize[cnt];
      }
      return (dim == 1 ? false : true);
    }

    virtual ObjectRef clone()
    {
        return ObjectRef(new Tensor(*this));
    }
    
    virtual void printOn(std::ostream& out) const
    {
        out << "<Tensor " << std::endl;
        out << "<vsize ";
	for(int cnt = 0; cnt < vsize.size(); cnt++){
	  out << vsize[cnt] << " ";
	}
	out << "> " << std::endl;
        // out << "<data ";
	// for(int cnt = 0; cnt < data.size(); cnt++){
	//   out << data[cnt] << " ";
	// }
	// out << ">" << std::endl;
        out << " >" << std::endl;
    }

    void readFrom(std::istream &in)
    {
    }
    
    friend std::istream& operator>>(std::istream &in, Tensor& src);
};

inline std::istream& operator>>(std::istream &in, Tensor& src)
{
    if (!isValidType(in, "Tensor")) {
        return in;
    }
    src.readFrom(in);
    return in;
}

} // namespace FD

#endif
