#include "lighting.hlsli"
#include "consts.shared"

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    uint     gMaterialIndex;
    uint __PerObjectPad1;
    uint __PerObjectPad2;
    uint __PerObjectPad3;
};

cbuffer cbPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3   gEyePosW;
    int      __PassPad1;
    DirectionalLight gDl[MAX_DIR_LIGHTS_COUNT];
    PointLight       gPl[MAX_PNT_LIGHTS_COUNT];
    SpotLight        gSl[MAX_SPT_LIGHTS_COUNT];
    uint gDirLightsCount;
    uint gPntLightsCount;
    uint gSptLightsCount;
    int  __PassPad2;
};

struct MaterialData
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float  FresnelR0;
    float  Roughness;
    float  __MaterialDataPad1;
    float  __MaterialDataPad2;
};

StructuredBuffer<MaterialData> gMaterialsData : register(t0);
