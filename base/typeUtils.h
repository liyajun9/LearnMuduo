#ifndef LINUXSERVER__BASE_TYPES_H
#define LINUXSERVER__BASE_TYPES_H

#include <cstdint>
#include <cstring>
#include <string>

#ifndef NDEBUG
#include <cassert>
#endif

namespace ybase
{
inline void memZero(void* p, size_t n)
{
  memset(p, 0, n);
}

//for upcast: to Super or to remove const
template<typename To, typename From>
inline To implicit_cast(From const &f)
{
  return f;
}

//for downcast: to Sub
template<typename To, typename From>
inline To down_cast(From* f)                     // so we only accept pointers
{
  // Ensures that To is a sub-type of From *.  This test is here only
  // for compile-time type checking, and has no overhead in an
  // optimized build at run-time, as it will be optimized away
  // completely.
  if (false){
    implicit_cast<From*, To>(0);
  }

#if !defined(NDEBUG) && !defined(GOOGLE_PROTOBUF_NO_RTTI)
  assert(f == NULL || dynamic_cast<To>(f) != NULL);  // RTTI: debug mode only!
#endif
  return static_cast<To>(f);
}

}  // namespace ybase

#endif  // LINUXSERVER_BASE_TYPES_H
