#include "cb.hlsli"
#include "normalTex_InputLayout.hlsli"

float4 main(VertexOut pin) : SV_Target
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

    float3 toEye = normalize(gEyePosW - pin.PosW);
    ComputeLights(mat, gDl, gPl, gSl, gDirLightsCount, gPntLightsCount, gSptLightsCount, pin.PosW, pin.NormalW, toEye, ambient, diffuse, specular);

    float4 color;
    color.rgb = mat.Diffuse.a * (ambient + diffuse) + specular;
    color.a = mat.Diffuse.a;
    return color;
}