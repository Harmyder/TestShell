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

#include "Pile\Types.h"

#ifdef _DEBUG
#include "Pile\Print\DumpDirectX.h"
#include "Pile\Print\DebugPrint.h"
#endif
