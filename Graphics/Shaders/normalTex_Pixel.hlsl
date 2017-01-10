#include "cb.hlsli"
#include "normalTex_InputLayout.hlsli"

float4 main(VertexOut pin) : SV_Target
{
    MaterialData matData = gMaterialsData[gMaterialIndex];

    const float shininess = (1.0f - matData.Roughness) * 256.f;
    Material mat;
    mat.Ambient = matData.Ambient;
    mat.Diffuse = matData.Diffuse;
    mat.Specular = matData.Specular;
    mat.FresnelR0 = float3(matData.FresnelR0, matData.FresnelR0, matData.FresnelR0);
    mat.Shininess = shininess;

    float3 ambient;
    float3 diffuse;
    float3 specular;

    float3 toEye = normalize(gEyePosW - pin.PosW);
    ComputeLights(mat, gDl, gPl, gSl, gDirLightsCount, gPntLightsCount, gSptLightsCount, pin.PosW, pin.NormalW, toEye, ambient, diffuse, specular);

    float4 color;
    color.rgb = mat.Diffuse.a * (ambient + diffuse) + specular;
    color.a = mat.Diffuse.a;
    return color;
}