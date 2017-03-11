// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue

#include <cassert>
#include <vector>
#include <memory>
#include <Windows.h>

#include "Common\Types.h"

#ifdef _DEBUG
#include "Common\Print\DumpDirectX.h"
#include "Common\Print\DebugPrint.h"
#endif
