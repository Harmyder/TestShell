#ifndef __PARTICLE_IL__
#define __PARTICLE_IL__

struct VertexIn
{
    float3 PosL : POSITION;
};

struct VertexOut
{
    float3 PosW : POSITION;
    float4 PosH    : SV_POSITION;
};

struct GeoOut
{
    float3 PosW    : POSITION;
    float4 PosH    : SV_POSITION;
    float3 NormalW : NORMAL;
    float2 TexC    : TEXCOORD;
};

#endif
