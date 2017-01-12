#ifndef __NORMALTEX__
#define __NORMALTEX__

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC    : TEXCOORD;
};

struct VertexOut
{
    float4 PosH    : SV_POSITION;
    float3 PosW    : POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXCOORD;
#ifdef SETTINGS_LIGHTING_INSTANCED
    nointerpolation uint MatIndex : MATINDEX;
#endif
};

#endif
