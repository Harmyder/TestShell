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

#define NOMINMAX
#include <Windows.h>

#include "Common\Types.h"

#ifdef _DEBUG
#include "Common\Print\DumpDirectX.h"
#include "Common\Print\DebugPrint.h"
#endif

// Bullet libs, used in Exploring_Bullet
#if defined(_DEBUG)
    #pragma comment(lib, "Bullet3Collision_Debug.lib")
    #pragma comment(lib, "Bullet3Dynamics_Debug.lib")
    #pragma comment(lib, "Bullet3Common_Debug.lib")
    #pragma comment(lib, "BulletCollision_Debug.lib")
    #pragma comment(lib, "BulletDynamics_Debug.lib")
    #pragma comment(lib, "BulletSoftBody_Debug.lib")
    #pragma comment(lib, "LinearMath_Debug.lib")
#else
    #pragma comment(lib, "Bullet3Collision_Release.lib")
    #pragma comment(lib, "Bullet3Dynamics_Release.lib")
    #pragma comment(lib, "Bullet3Common_Release.lib")
    #pragma comment(lib, "BulletCollision_Release.lib")
    #pragma comment(lib, "BulletDynamics_Release.lib")
    #pragma comment(lib, "BulletSoftBody_Release.lib")
    #pragma comment(lib, "LinearMath_Release.lib")
#endif

