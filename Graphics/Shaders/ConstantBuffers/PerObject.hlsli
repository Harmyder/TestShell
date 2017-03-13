#ifndef __PER_OBJECT__
#define __PER_OBJECT__

cbuffer cbPerObject : register(REGISTER_CB_PER_OBJECT)
{
    float4x3 gWorld;
    uint     gMaterialIndex;
    uint __PerObjectPad1;
    uint __PerObjectPad2;
    uint __PerObjectPad3;
};

#endif
