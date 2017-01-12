// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#pragma warning(disable:4238)

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#include <DirectXMath.h>
using DirectX::XMFLOAT3;
using DirectX::XMFLOAT4;
using DirectX::XMFLOAT3X3;
using DirectX::XMFLOAT4X3;
using DirectX::XMFLOAT4X4;
using DirectX::XMVECTOR;
using DirectX::FXMVECTOR;
using DirectX::XMMATRIX;
using DirectX::FXMMATRIX;

#include "Pile\Types.h"

#define _USE_MATH_DEFINES
#include <math.h>

#include <algorithm>
#include <memory>
#include <vector>
#include <tuple>

#include "SDK\GeometryTypes.h"

