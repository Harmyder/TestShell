#include "cb.hlsli"

float4 main(VertexOut vin) : SV_Target
{
    const float shininess = (1.0f - gRoughness) * 256.f;
    Material mat;
    mat.Ambient = gAmbient;
    mat.Diffuse = gDiffuse;
    mat.Specular = gSpecular;
    mat.FresnelR0 = float3(gFresnelR0, gFresnelR0, gFresnelR0);
    mat.Shininess = shininess;

    float3 ambient;
    float3 diffuse;
    float3 specular;

    float3 toEye = normalize(gEyePosW - vin.PosW);
    ComputeLights(mat, gDl, gPl, gSl, gDirLightsCount, gPntLightsCount, gSptLightsCount, vin.PosW, vin.NormalW, toEye, ambient, diffuse, specular);

    float4 color = float4(ambient + diffuse, 1.f);
    return color;
}