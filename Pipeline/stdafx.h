// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#define NOMINMAX
#include <windows.h>

#include <vector>
#include <map>
#include <algorithm>
#include <memory>

#include "Common\DirectXInclude.h"

#include "Common\Types.h"
#include "Common\Consts.h"
