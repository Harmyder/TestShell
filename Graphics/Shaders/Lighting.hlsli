#include "consts.shared"

struct Material
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float3 FresnelR0;
    float  Shininess; 
};

struct DirectionalLight
{
    float3 Strength;
    float  pad1;
    float3 Direction;
    float  pad2;
};

struct PointLight
{
    float3 Strength;
    float  Range;
    float3 Position;
    float  pad1;
    float3 Attenuation;
    float  pad2;
};

struct SpotLight
{
    float3 Strength;
    float  Range;
    float3 Position;
    float  Spot;
    float3 Direction;
    float  pad1;
    float3 Attenuation;
    float  pad2;
};

float3 SchlickApprox(float3 normal, float3 lightVec, float3 fresnelR0)
{
    float3 fresnel = fresnelR0  + (1.f - fresnelR0) * pow(1.f - saturate(dot(lightVec, normal)), 5);
    return fresnel;
}

void BlinnPhong(float3 lightStrength, float3 lightVec, float3 normal, float3 toEye, Material mat, out float3 diffuse, out float3 spec)
{
    float3 diffuseFactor = mat.Diffuse.rgb;
    diffuse = diffuseFactor * lightStrength;

    float3 halfVec = normalize(lightVec + toEye);
    float roughnessShare = pow(max(dot(halfVec, lightVec), 0), mat.Shininess) * (mat.Shininess + 8.f) / 8.f;
    float3 specFactor = SchlickApprox(normal, halfVec, mat.FresnelR0) * roughnessShare;
    spec = specFactor * lightStrength;
}

void ComputeDirectionalLight(Material mat,
    DirectionalLight light,
    float3 normal,
    float3 toEye,
    out float3 ambient,
    out float3 diffuse,
    out float3 spec)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    spec    = float3(0.0f, 0.0f, 0.0f);

    ambient = mat.Ambient.rgb * light.Strength;

    float3 lightVec = -light.Direction;

    float lambert = max(dot(normal, lightVec), 0);
    float3 lightStrength = lambert * light.Strength;    
    BlinnPhong(lightStrength, lightVec, normal, toEye, mat, diffuse, spec);
}

void ComputePointLight(Material mat,
    PointLight light,
    float3 pos,
    float3 normal,
    float3 toEye,
    out float3 ambient,
    out float3 diffuse,
    out float3 spec)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    spec    = float3(0.0f, 0.0f, 0.0f);

    float3 lightVec = light.Position - pos;

    float d = length(lightVec);
    if (d > light.Range)
        return;

    // Normalize the light vector.
    lightVec /= d;

    ambient = mat.Ambient * light.Strength;

    float lambert = max(dot(normal, lightVec), 0);
    float lightStrength = lambert * light.Strength;
    BlinnPhong(lightStrength, lightVec, normal, toEye, mat, diffuse, spec);

    float attenuate = 1.0f / dot(light.Attenuation, float3(1.0f, d, d*d));
    diffuse *= attenuate;
    spec *= attenuate;
}

void ComputeSpotLight(Material mat,
    SpotLight light,
    float3 pos,
    float3 normal,
    float3 toEye,
    out float3 ambient,
    out float3 diffuse,
    out float3 spec)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    spec    = float3(0.0f, 0.0f, 0.0f);

    float3 lightVec = light.Position - pos;

    float d = length(lightVec);
    if (d > light.Range)
        return;

    lightVec /= d;

    ambient = mat.Ambient * light.Strength;

    float lambert = max(dot(normal, lightVec), 0);
    float3 lightStrength = lambert * light.Strength;
    BlinnPhong(lightStrength, lightVec, normal, toEye, mat, diffuse, spec);

    float spot = pow(max(dot(-lightVec, light.Direction), 0.0f), light.Spot);
    float attenuate = spot / dot(light.Attenuation, float3(1.0f, d, d*d));
    ambient *= spot;
    diffuse *= attenuate;
    spec *= attenuate;
}

void ComputeLights(Material mat,
    DirectionalLight dl[MAX_DIR_LIGHTS_COUNT],
    PointLight pl[MAX_PNT_LIGHTS_COUNT],
    SpotLight sl[MAX_SPT_LIGHTS_COUNT],
    int dirLightsCount,
    int pntLightsCount,
    int sptLightsCount,
    float3 pos,
    float3 normal,
    float3 toEye,
    out float3 ambient,
    out float3 diffuse,
    out float3 specular)
{
    ambient = float3(0.0f, 0.0f, 0.0f);
    diffuse = float3(0.0f, 0.0f, 0.0f);
    specular = float3(0.0f, 0.0f, 0.0f);

    float3 A, D, S;
    
    for (int i = 0; i < dirLightsCount; ++i) {
        ComputeDirectionalLight(mat, dl[i], normal, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        specular += S;
    }

    for (int i = 0; i < pntLightsCount; ++i) {
        ComputePointLight(mat, pl[i], pos, normal, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        specular += S;
    }

    for (int i = 0; i < sptLightsCount; ++i) {
        ComputeSpotLight(mat, sl[i], pos, normal, toEye, A, D, S);
        ambient += A;
        diffuse += D;
        specular += S;
    }
}
