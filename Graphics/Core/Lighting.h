#pragma once
#include <DirectXMath.h>
#include <string>
#include "Core\GraphicsCore.h"

namespace Graphics
{
    struct DirectionalLight
    {
        DirectionalLight(DirectX::XMFLOAT3 direction) :
            Direction(direction)
        {}

        DirectX::XMFLOAT3 Direction;
        float pad;
    };

    struct PointLight
    {
        PointLight(DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 attenuation) :
            Position(position),
            Range(range),
            Attenuation(attenuation)
        {}

        DirectX::XMFLOAT3 Position;
        float Range;
        DirectX::XMFLOAT3 Attenuation;
        float pad;
    };

    struct SpotLight
    {
        SpotLight(DirectX::XMFLOAT3 position, float range, DirectX::XMFLOAT3 direction, float spot, DirectX::XMFLOAT3 attenuation) :
            Position(position),
            Range(range),
            Direction(direction),
            Spot(spot),
            Attenuation(attenuation)
        {}

        DirectX::XMFLOAT3 Position;
        float Range;
        DirectX::XMFLOAT3 Direction;
        float Spot;
        DirectX::XMFLOAT3 Attenuation;
        float m_Pad;
    };

    struct Material
    {
        Material(const std::string& name, uint32 cbIndex) :
            name_(name),
            cbIndex_(cbIndex),
            dirtyFramesCount_(0)
        {}

        enum class Type {
            kTurquesa,
            kEmerald,
            kJade,
            kObsidian,
            kSilver,
        };
        static std::unique_ptr<Material> Create(Type type, const std::string& name, uint32 cbIndex);

        uint32 CbIndex() const { return cbIndex_; }
        bool IsDirty() const { return dirtyFramesCount_ != 0; }
        void DecreaseDirtyFramesCount() { --dirtyFramesCount_; }

        void Update(
            const DirectX::XMFLOAT4& ambient,
            const DirectX::XMFLOAT4& diffuse,
            const DirectX::XMFLOAT4& specular,
            float fresnelR0,
            float roughness)
        {
            ambient_ = ambient;
            diffuse_ = diffuse;
            specular_ = specular;
            fresnelR0_ = fresnelR0;
            roughness_ = roughness;
            dirtyFramesCount_ = GraphicsCore::GetInstance().GetFrameResourcesCount();
        }

        const DirectX::XMFLOAT4& GetAmbient() const { return ambient_; }
        const DirectX::XMFLOAT4& GetDiffuse() const { return diffuse_; }
        const DirectX::XMFLOAT4& GetSpecular() const { return specular_; }
        float GetFresnelR0() const { return fresnelR0_; }
        float GetRoughness() const { return roughness_; }

    private:
        const std::string name_;
        const int cbIndex_;
        int dirtyFramesCount_;

        DirectX::XMFLOAT4 ambient_;
        DirectX::XMFLOAT4 diffuse_;
        DirectX::XMFLOAT4 specular_;
        float fresnelR0_;
        float roughness_;
    };
}
