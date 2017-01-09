#include "lighting.hlsli"
#include "consts.shared"

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
};

cbuffer cbMaterial : register(b1)
{
    float4 gAmbient;
    float4 gDiffuse;
    float4 gSpecular;
    float  gFresnelR0;
    float  gRoughness;
}; 

cbuffer cbPass : register(b2)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float3   gEyePosW;
    int      pad;
    DirectionalLight gDl[MAX_DIR_LIGHTS_COUNT];
    PointLight       gPl[MAX_PNT_LIGHTS_COUNT];
    SpotLight        gSl[MAX_SPT_LIGHTS_COUNT];
    uint gDirLightsCount;
    uint gPntLightsCount;
    uint gSptLightsCount;
    int  pad2;
};

