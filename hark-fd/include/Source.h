/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#ifndef __SOURCE_H__
#define __SOURCE_H__

#include "Object.h"
#include "ObjectParser.h"
#include "misc.h"
#include <cmath>

namespace FD {



class Source : public Object {
public:
    enum TFDim {TFHeight, TFDirection, TFRange};
    enum CompareMode {DEG, TFINDEX};
    float x[3];
    float power;
    int id;
    float remaining;
    int count;
    int last_found_count;

    //inserted for index search
    int tfindex[3];
    int tfindex_max[3];
    int tfindex_min[3];
    CompareMode compare_mode;

    Source() {
    }

    Source(const Source& src) {
      x[0] = src.x[0];
      x[1] = src.x[1];
      x[2] = src.x[2];
      power = src.power;
      id = src.id;
      remaining = src.remaining;
      count = src.count;
      last_found_count = src.last_found_count;

      //inserted for index search
      for(int i=0; i<3; i++){
        tfindex[i] = src.tfindex[i];
        tfindex_min[i] = src.tfindex_min[i];
        tfindex_max[i] = src.tfindex_max[i];
      }
      compare_mode = src.compare_mode;
    }

    int tfindexDistance(const int* y) const
    {
      int maxdist = 0;
      for(int i=0; i<3; i++){
        int tmpdist;
        if(tfindex[i] > y[i]){
          tmpdist = min(abs(tfindex[i] - y[i]),
                        abs(tfindex[i] - tfindex_max[i]) + abs(y[i] - tfindex_min[i]));
        }else{
          tmpdist = min(abs(tfindex[i] - y[i]),
                        abs(tfindex[i] - tfindex_min[i]) + abs(y[i] - tfindex_max[i]));
        }
        maxdist = (maxdist < tmpdist)?tmpdist:maxdist;
      }
      return maxdist;
    }

    int tfindexDistance(const Source& src)
    {
      int maxdist = 0;
      for(int i=0; i<3; i++){
        int tmpdist;
        if(tfindex[i] > src.tfindex[i]){
          tmpdist = min(abs(tfindex[i] - src.tfindex[i]),
                        abs(tfindex[i] - tfindex_max[i]) + abs(src.tfindex[i] - tfindex_min[i]));
        }else{
          tmpdist = min(abs(tfindex[i] - src.tfindex[i]),
                        abs(tfindex[i] - tfindex_min[i]) + abs(src.tfindex[i] - tfindex_max[i]));
        }
        maxdist = (maxdist < tmpdist)?tmpdist:maxdist;
      }
      return maxdist;
    }


    float angle(const float* y) const
    {
      float prod = (x[0]*y[0]+x[1]*y[1]+x[2]*y[2]) / sqrtf(x[0]*x[0]+x[1]*x[1]+x[2]*x[2]) / sqrtf(y[0]*y[0]+y[1]*y[1]+y[2]*y[2]);
        if (prod < -1) {
            prod = -1;
        }
        if (prod > 1) {
            prod = 1;
        }
        return acosf(prod);
    }

    float angle(const Source& src)
    {
      float prod = (x[0]*src.x[0]+x[1]*src.x[1]+x[2]*src.x[2])
	/ sqrtf(x[0]*x[0]+x[1]*x[1]+x[2]*x[2])
	/ sqrtf(src.x[0]*src.x[0]+src.x[1]*src.x[1]+src.x[2]*src.x[2]);
      return acosf(prod);
    }

    virtual ObjectRef clone()
    {
        return ObjectRef(new Source(*this));
    }

    virtual void printOn(std::ostream& out) const
    {
        out << "<Source " << std::endl;
        out << "<x " << x[0] << " " << x[1] << x[2] << " >" << std::endl;
        out << "<power " << power << " >" << std::endl;
        out << "<id " << id << " >" << std::endl;
        out << ">" << std::endl;
    }

    void readFrom(std::istream &in)
    {
    }

    friend std::istream& operator>>(std::istream &in, Source& src);
};

inline std::istream& operator>>(std::istream &in, Source& src)
{
    if (!isValidType(in, "Source")) {
        return in;
    }
    src.readFrom(in);
    return in;
}

} // namespace FD

#endif
