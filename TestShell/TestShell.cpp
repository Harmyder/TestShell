#include "stdafx.h"

#include "Game.h"

#ifdef _DEBUG
// Finding Memory Leaks Using the CRT Library
// http://msdn.microsoft.com/en-us/library/x98tx3cf.aspx
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif // _DEBUG

int main()
{
#ifdef _DEBUG
    _CrtMemState initialMemoryState;
    _CrtMemCheckpoint(&initialMemoryState);

    // To set breakpoint for a certain memory allocation
    //_CrtSetBreakAlloc(360);
#endif // _DEBUG
//    if (DirectX::XMVerifyCPUSupport())
    {
        Game game;
        game.Run(GetModuleHandle(NULL));
    }
    //else
    //{
    //    OutputDebugString("ERROR: DirectXMath library doesn't support current platform.\n");
    //    return 1;
    //}

#ifdef _DEBUG
    _CrtMemState finalMemoryState;
    _CrtMemCheckpoint(&finalMemoryState);

    _CrtMemState differenceMemoryState;

    if (_CrtMemDifference(&differenceMemoryState, &initialMemoryState, &finalMemoryState))
    {
        OutputDebugString("ERROR: Memore leaks.\n");
        _CrtMemDumpAllObjectsSince(&initialMemoryState);
    }
    else
    {
        OutputDebugString("SUCCEEDED: No memory leaks.\n");
    }
#endif // _DEBUG
    return 0;
}

