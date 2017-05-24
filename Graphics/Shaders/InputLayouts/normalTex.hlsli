#ifndef __NORMALTEX_IL__
#define __NORMALTEX_IL__

struct VertexIn
{
    float3 PosL    : POSITION;
    float3 NormalL : NORMAL;
    float2 TexC    : TEXCOORD;
};

struct VertexOut
{
    float3 PosW    : POSITION;
    float4 PosH    : SV_POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXCOORD;
#ifdef SETTINGS_LIGHTING_INSTANCED
    nointerpolation uint MatIndex : MATINDEX;
#endif
};

#endif
