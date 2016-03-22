/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#ifndef __VECSTRVECFLT_H__
#define __VECSTRVECFLT_H__

#include "Object.h"
#include "ObjectParser.h"
#include "misc.h"
#include <cmath>

namespace FD {

class VecStrVecFlt : public Object {
  public:
    std::vector<std::string> vecstr;
    std::vector<float> vecflt;

    VecStrVecFlt() {
    }
    
    VecStrVecFlt(const VecStrVecFlt& input) {
      vecstr.resize(input.vecstr.size());
      for(int cnt = 0; cnt < input.vecstr.size(); cnt++){
	vecstr[cnt] = input.vecstr[cnt];
      }
      vecflt.resize(input.vecflt.size());
      for(int cnt = 0; cnt < input.vecflt.size(); cnt++){
	vecflt[cnt] = input.vecflt[cnt];
      }
    }

    ~VecStrVecFlt() {
    }

    virtual ObjectRef clone()
    {
        return ObjectRef(new VecStrVecFlt(*this));
    }
    
    virtual void printOn(std::ostream& out) const
    {
        out << "<VecStrVecFlt " << std::endl;
        out << "<vecstr ";
	for(int cnt = 0; cnt < vecstr.size(); cnt++){
	  out << vecstr[cnt] << " ";
	}
	out << "> " << std::endl;
        // out << "<vecflt ";
	// for(int cnt = 0; cnt < vecflt.size(); cnt++){
	//   out << vecflt[cnt] << " ";
	// }
	// out << ">" << std::endl;
        out << " >" << std::endl;
    }

    void readFrom(std::istream &in)
    {
    }

    friend std::istream& operator>>(std::istream &in, VecStrVecFlt& src);
    
};

inline std::istream& operator>>(std::istream &in, VecStrVecFlt& src)
{
    if (!isValidType(in, "VecStrVecFlt")) {
        return in;
    }
    src.readFrom(in);
    return in;
}

} // namespace FD

#endif
