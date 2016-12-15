#pragma once

typedef unsigned __int64 uint64;
typedef unsigned __int32 uint32;
typedef unsigned __int16 uint16;

typedef __int64 int64;
typedef __int32 int32;
typedef __int16 int16;

#ifdef _WIN64
    typedef unsigned __int64 uint_t;
    typedef __int64 int_t;
#else
#ifdef _WIN32
    typedef unsigned __int32 uint_t;
    typedef __int32 int_t;
#endif // _WIN32
#endif //_WIN64

