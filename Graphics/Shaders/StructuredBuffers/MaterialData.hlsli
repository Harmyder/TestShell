#ifndef __MATERIAL_DATA__
#define __MATERIAL_DATA__

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

StructuredBuffer<MaterialData> gMaterialsData : register(REGISTER_SB_MATERIAL_DATA);

#endif
