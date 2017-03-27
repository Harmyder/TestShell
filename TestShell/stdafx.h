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
#include <Windows.h>

#include "Common\Types.h"

#ifdef _DEBUG
#include "Common\Print\DumpDirectX.h"
#include "Common\Print\DebugPrint.h"
#endif

// Bullet libs, used in Exploring_Bullet
#if defined(_DEBUG)
    #pragma comment(lib, "Bullet3Collision_x64_Debug.lib")
    #pragma comment(lib, "Bullet3Dynamics_x64_Debug.lib")
    #pragma comment(lib, "Bullet3Common_x64_Debug.lib")
    #pragma comment(lib, "BulletCollision_x64_Debug.lib")
    #pragma comment(lib, "BulletDynamics_x64_Debug.lib")
    #pragma comment(lib, "LinearMath_x64_Debug.lib")
#else
    #pragma comment(lib, "Bullet3Collision_x64_Release.lib")
    #pragma comment(lib, "Bullet3Dynamics_x64_Release.lib")
    #pragma comment(lib, "Bullet3Common_x64_Release.lib")
    #pragma comment(lib, "BulletCollision_x64_Release.lib")
    #pragma comment(lib, "BulletDynamics_x64_Release.lib")
    #pragma comment(lib, "LinearMath_x64_Release.lib")
#endif

