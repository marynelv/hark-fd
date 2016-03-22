/*
 * Copyright 2008 Kyoto University and Honda Motor Co.,Ltd.
 * All rights reserved.
 * HARK was developed by researchers in Okuno Laboratory
 * at the Kyoto University and Honda Research Institute Japan Co.,Ltd.
 */
#include "Object.h"
#include <map>
#include <iostream>

namespace FD {

template <class _Key, class _Tp>
class Map : public Object , public std::map<_Key, _Tp> {
public:
    Map() {}
    Map(const Map &m) : std::map<_Key, _Tp> (m) {}
    
    virtual void printOn(std::ostream& out = std::cout) const {}
};

}
