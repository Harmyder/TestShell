// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue
#pragma warning(disable:4324) // structure was padded due to __declspec(align())

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#include <Windows.h>

#include <comdef.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#include <d3d12.h>
#include <dxgi1_4.h>
#include "3rdParty\d3dx12.h"

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <string>
#include <memory>
#include <mutex>

#include "Pile/Types.h"
#include "Pile/Print/DebugPrint.h"
#include "Pile/DirectXInclude.h"
#include "Utility.h"
