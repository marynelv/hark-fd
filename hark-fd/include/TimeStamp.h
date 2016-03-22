/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */

#ifndef ___TIMESTAMP_H___
#define ___TIMESTAMP_H___

#include <sys/time.h>

#include "Object.h"
#include "ObjectParser.h"
#include "misc.h"

namespace FD {

class TimeStamp : public Object {
public:
  struct timeval time;

public:
  TimeStamp() {}
  TimeStamp(struct timeval t) : time(t) {}
  TimeStamp(long int sec, long int usec) {
    time.tv_sec = sec;
    time.tv_usec = usec;
  }

  void setTime(struct timeval t) {
    time = t; 
  }
  void setTime(long int sec, long int usec) {
    time.tv_sec = sec;
    time.tv_usec = usec;
  }

  void setLocalTime() {
    gettimeofday(&time, NULL);
  }


  virtual void printOn(std::ostream& out) const
  {
    out << "<TimeStamp <sec " << time.tv_sec << "> <usec " << time.tv_usec << "> >";
  }

  void readFrom(std::istream &in)
  {
  }
  
  friend std::istream& operator>>(std::istream &in, TimeStamp& src);
};

inline std::istream& operator>>(std::istream &in, TimeStamp& src)
{
    if (!isValidType(in, "TimeStamp")) {
        return in;
    }
    src.readFrom(in);
    return in;
}

}


#endif
