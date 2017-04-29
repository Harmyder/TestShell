float4 main(VertexOut pin) : SV_Target
{
#ifdef SETTINGS_LIGHTING_INSTANCED
    MaterialData matData = gMaterialsData[pin.MatIndex];
#else
    MaterialData matData = gMaterialsData[gMaterialIndex];
#endif

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
    float3 texDiffuse = gDiffuseMap.Sample(gSamLinear, pin.TexC).rgb;
    color.rgb = mat.Diffuse.a * ((ambient + diffuse) * texDiffuse) + specular;
    color.a = mat.Diffuse.a;
    return color;
}
