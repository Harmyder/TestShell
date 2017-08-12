// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#define NOMINMAX
#include <Windows.h>

#include <type_traits>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <functional>

#include "Types.h"
#include "Consts.h"

#ifdef _DEBUG
    #include "Print/DebugPrint.h"
#endif
