#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>

void Printf(const char* formatstring, ...)
{
    va_list args;
    va_start(args, formatstring);
 
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    vsnprintf_s(buf, _countof(buf), _TRUNCATE, formatstring, args);

    OutputDebugString(buf);

    va_end(args);
}

//***************************************************************************************
#define PrintErrorOnCreation(pointer, objectName) \
    if (!pointer) \
        OutputDebugString("YolaFbxImporter: Unable to create "##objectName##".\n"); \
    else \
        OutputDebugString("YolaFbxImporter: "##objectName##" creation succeeded .\n")

//***************************************************************************************
#define PrintErroOnFunctionCall(result, failureMark, functionName) \
    if (result == failureMark) \
        OutputDebugString("YolaFbxImporter: "##functionName##" failed.\n"); \
    else \
        OutputDebugString("YolaFbxImporter: "##functionName##" succeeded.\n")
