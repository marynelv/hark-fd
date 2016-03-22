#ifndef ___ENDIAN_HPP___
#define ___ENDIAN_HPP___

#include <algorithm>
#include <iterator>

#include <boost/type_traits.hpp>

#if 0
typedef unsigned short BYTE;
typedef unsigned int DWORD;

inline BYTE swap_endian(BYTE n) {
  return ((BYTE)((n & 0x00ff) << 8) |
		  (BYTE)((n & 0xff00) >> 8));
}

inline DWORD swap_endian(DWORD n) {
  return (((n & 0x000000ffUL) << 24) | ((n & 0x0000ff00UL) << 8) |
		  ((n & 0x00ff0000UL) >> 8) | ((n & 0xff000000UL) >> 24));
}
#endif

template <size_t Size>
void swap_endian(void* p);

template <>
void swap_endian<2>(void* p) {
    char* pb = (char*)p;
    std::swap(pb[0], pb[1]);
}

template <>
void swap_endian<4>(void* p) {
    char* pb = (char*)p;
    std::swap(pb[0], pb[3]);
    std::swap(pb[1], pb[2]);
}

template <>
void swap_endian<8>(void* p) {
    char* pb = (char*)p;
    std::swap(pb[0], pb[7]);
    std::swap(pb[1], pb[6]);
    std::swap(pb[2], pb[5]);
    std::swap(pb[3], pb[4]);
}

template <typename Type>
Type swap_endian(Type v) {
    Type result = v;
    swap_endian<sizeof(v)>((void*)&result);
    return result;
}

template <typename InputIterator>
void swap_endian(InputIterator first, InputIterator last) {
    typedef typename boost::remove_cv<
        typename std::iterator_traits<InputIterator>::value_type>::type
            input_type;

    for (; first != last; ++first) {
        swap_endian<sizeof(input_type)>((void*)&(*first));
    }
}


#endif
