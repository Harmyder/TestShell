#ifdef SETTINGS_PARTICLES
float4 main(GeoOut pin) : SV_Target
#else
float4 main(VertexOut pin) : SV_Target
#endif
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
    float4 texDiffuse = gDiffuseMap.Sample(gSamLinear, pin.TexC);
    color.rgb = mat.Diffuse.a * ((ambient + diffuse) * texDiffuse.rgb) + specular;
    color.a = mat.Diffuse.a * texDiffuse.a;
    return color;
}
