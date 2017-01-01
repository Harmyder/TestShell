#include "stdafx.h"

#include "Core\Lighting.h"

using namespace std;

namespace Graphics
{
    static constexpr float CalcFresnelR0(float ior) {
        return (ior - 1) / (ior + 1) * (ior - 1) / (ior + 1);
    }

    // Roughness - guessed values
    unique_ptr<Material> Material::Create(Type type, const string& name, uint32 cbIndex) {
        auto ret = make_unique<Material>(name, cbIndex);
        XMFLOAT4 ambient;
        XMFLOAT4 diffuse;
        XMFLOAT4 specular;
        float fresnelR0 = 0;
        float roughness = 0;
        switch (type) {
        case Type::kRed:
            ambient = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
            diffuse = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
            specular = XMFLOAT4(1.f, 0.f, 0.f, 1.f);
            fresnelR0 = CalcFresnelR0(1.61f);
            roughness = 0.95f;
            break;
        case Type::kGreen:
            ambient = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
            diffuse = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
            specular = XMFLOAT4(0.f, 1.f, 0.f, 1.f);
            fresnelR0 = CalcFresnelR0(1.61f);
            roughness = 0.95f;
            break;
        case Type::kBlue:
            ambient = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
            diffuse = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
            specular = XMFLOAT4(0.f, 0.f, 1.f, 1.f);
            fresnelR0 = CalcFresnelR0(1.61f);
            roughness = 0.95f;
            break;
        case Type::kTurquesa:
            ambient = XMFLOAT4(0.1f, 0.18725f, 0.1745f, 1.f);
            diffuse = XMFLOAT4(0.396f, 0.74151f, 0.69102f, 1.f);
            specular = XMFLOAT4(0.297254f, 0.30829f, 0.306678f, 1.f);
            fresnelR0 = CalcFresnelR0(1.61f);
            roughness = 0.95f;
            break;
        case Type::kEmerald:
            ambient = XMFLOAT4(0.0215f, 0.1745f, 0.0215f, 1.f);
            diffuse = XMFLOAT4(0.07568f, 0.61424f, 0.07568f, 1.f);
            specular = XMFLOAT4(0.633f, 0.727811f, 0.633f, 1.f);
            fresnelR0 = CalcFresnelR0(1.576f);
            roughness = 0.7f;
            break;
        case Type::kJade:
            ambient = XMFLOAT4(0.135f, 0.2225f, 0.1575f, 1.f);
            diffuse = XMFLOAT4(0.54f, 0.89f, 0.63f, 1.f);
            specular = XMFLOAT4(0.316228f, 0.316228f, 0.316228f, 1.f);
            fresnelR0 = CalcFresnelR0(1.59f);
            roughness = 0.95f;
            break;
        case Type::kObsidian:
            ambient = XMFLOAT4(0.05375f, 0.05f, 0.06625f, 1.f);
            diffuse = XMFLOAT4(0.18275f, 0.17f, 0.22525f, 1.f);
            specular = XMFLOAT4(0.332741f, 0.328634f, 0.346435f, 1.f);
            fresnelR0 = CalcFresnelR0(1.5f);
            roughness = 0.85f;
            break;
        case Type::kSilver:
            ambient = XMFLOAT4(0.19225f, 0.19225f, 0.19225f, 1.f);
            diffuse = XMFLOAT4(0.50754f, 0.50754f, 0.50754f, 1.f);
            specular = XMFLOAT4(0.508273f, 0.508273f, 0.508273f, 1.f);
            fresnelR0 = CalcFresnelR0(1.35f);
            roughness = 0.65f;
            break;
        default:
            throw "Unknown material";
        }
        ret->Update(ambient, diffuse, specular, fresnelR0, roughness);
        return ret;
    }

    DirectionalLight* LightsHolder::CreateDirLightBillet() {
        assert(dirLightsCount_ < MAX_DIR_LIGHTS_COUNT);
        dirLights_[dirLightsCount_] = make_unique<DirectionalLight>(dirLightsCount_);
        return dirLights_[dirLightsCount_++].get();
    }
    PointLight* LightsHolder::CreatePntLightBillet() {
        assert(pntLightsCount_ < MAX_PNT_LIGHTS_COUNT);
        pntLights_[pntLightsCount_] = make_unique<PointLight>(pntLightsCount_);
        return pntLights_[pntLightsCount_++].get();
    }
    SpotLight* LightsHolder::CreateSptLightBillet() {
        assert(sptLightsCount_ < MAX_SPT_LIGHTS_COUNT);
        sptLights_[sptLightsCount_] = make_unique<SpotLight>(sptLightsCount_);
        return sptLights_[sptLightsCount_++].get();
    }
}