#ifndef ___TIME_H___
#define ___TIME_H___

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

#include "Object.h"
#include "ObjectParser.h"
#include "misc.h"

namespace FD {

class Time : public Object {
public:
  boost::posix_time::ptime time;

public:
  Time() {}
  Time(boost::posix_time::ptime t) : time(t) {}

  void setTime(boost::posix_time::ptime t) {
    time = t;
  }

  void setLocalTime() {
    time = boost::posix_time::microsec_clock::local_time();
  }


  virtual void printOn(std::ostream& out) const
  {
    out << "<Time <time " << time << "> >";
  }

  void readFrom(std::istream &in)
  {
  }
  
  friend std::istream& operator>>(std::istream &in, Time& src);
};

inline std::istream& operator>>(std::istream &in, Time& src)
{
    if (!isValidType(in, "Time")) {
        return in;
    }
    src.readFrom(in);
    return in;
}

}


#endif
