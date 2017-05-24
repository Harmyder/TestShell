#ifndef __PER_PASS__
#define __PER_PASS__

cbuffer cbPerPass : register(REGISTER_CB_PER_PASS)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
#if defined(SETTINGS_LIGHTING) || defined(SETTINGS_LIGHTING_INSTANCED) || defined(SETTINGS_PARTICLES)
    float3   gEyePosW;
    int      __PassPad1;
    DirectionalLight gDl[MAX_DIR_LIGHTS_COUNT];
    PointLight       gPl[MAX_PNT_LIGHTS_COUNT];
    SpotLight        gSl[MAX_SPT_LIGHTS_COUNT];
    uint gDirLightsCount;
    uint gPntLightsCount;
    uint gSptLightsCount;
    int  __PassPad2;
#endif
};

#endif
