// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once
#pragma warning(disable:4238) // nonstandard extension used : class rvalue used as lvalue

#include <cassert>
#include <vector>
#include <array>
#include <memory>
#include <algorithm>
#include <functional>
#include <random>
#include <numeric>

#define NOMINMAX
#include <Windows.h>

#include "Common\Types.h"

#ifdef _DEBUG
#include "Common\Print\DumpDirectX.h"
#include "Common\Print\DebugPrint.h"
#endif

// Bullet libs, used in Exploring_Bullet
#if defined(_DEBUG)
    #pragma comment(lib, "Bullet3Collision.lib")
    #pragma comment(lib, "Bullet3Dynamics.lib")
    #pragma comment(lib, "Bullet3Common.lib")
    #pragma comment(lib, "BulletCollision.lib")
    #pragma comment(lib, "BulletDynamics.lib")
    #pragma comment(lib, "BulletSoftBody.lib")
    #pragma comment(lib, "LinearMath.lib")
#else
    #pragma comment(lib, "Bullet3Collision.lib")
    #pragma comment(lib, "Bullet3Dynamics.lib")
    #pragma comment(lib, "Bullet3Common.lib")
    #pragma comment(lib, "BulletCollision.lib")
    #pragma comment(lib, "BulletDynamics.lib")
    #pragma comment(lib, "BulletSoftBody.lib")
    #pragma comment(lib, "LinearMath.lib")
#endif

