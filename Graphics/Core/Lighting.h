#pragma once
#include <DirectXMath.h>
#include <string>
#include "Shaders/consts.shared.h"
#include "Utility/BufferStuff.h"
#include "Common/Container/Dynarray.h"

namespace Graphics
{
    class DirectionalLight : public Utility::BufferEntryDirty
    {
    public:
        DirectionalLight(uint32 bufferIndex) :
            BufferEntryDirty(bufferIndex)
        {}

        void Update(
            DirectX::XMFLOAT3 strength,
            DirectX::XMFLOAT3 direction)
        {
            strength_ = strength;
            direction_ = direction;
            SetAllFramesDirty();
        }

        const DirectX::XMFLOAT3& Strength() const { return strength_; }
        const DirectX::XMFLOAT3& Direction() const { return direction_; }

    private:
        DirectX::XMFLOAT3 strength_;
        float pad1;
        DirectX::XMFLOAT3 direction_;
        float pad2;
    };

    class PointLight : public Utility::BufferEntryDirty
    {
    public:
        PointLight(uint32 bufferIndex) :
            BufferEntryDirty(bufferIndex)
        {}

        void Update(DirectX::XMFLOAT3 position) {
            position_ = position;
            SetAllFramesDirty();
        }

        void Update(
            DirectX::XMFLOAT3 strength,
            float range,
            DirectX::XMFLOAT3 position,
            DirectX::XMFLOAT3 attenuation)
        {
            strength_ = strength;
            range_ = range;
            position_ = position;
            attenuation_ = attenuation;
            SetAllFramesDirty();
        }

        const DirectX::XMFLOAT3& Strength() const { return strength_; }
        float Range() const { return range_; }
        const DirectX::XMFLOAT3& Position() const { return position_; }
        const DirectX::XMFLOAT3& Attenuation() const { return attenuation_; }

    private:
        DirectX::XMFLOAT3 strength_;
        float range_;
        DirectX::XMFLOAT3 position_;
        float pad1;
        DirectX::XMFLOAT3 attenuation_;
        float pad2;
    };

    class SpotLight : public Utility::BufferEntryDirty
    {
    public:
        SpotLight(uint32 bufferIndex) :
            BufferEntryDirty(bufferIndex)
        {}

        void Update(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 direction) {
            position_ = position;
            direction_ = direction;
            SetAllFramesDirty();
        }

        void Update(
            DirectX::XMFLOAT3 strength,
            float range,
            DirectX::XMFLOAT3 position,
            float spot,
            DirectX::XMFLOAT3 direction,
            DirectX::XMFLOAT3 attenuation)
        {
            strength_ = strength;
            range_ = range;
            position_ = position;
            spot_ = spot;
            direction_ = direction;
            attenuation_ = attenuation;
            SetAllFramesDirty();
        }

        const DirectX::XMFLOAT3& Strength() const { return strength_; }
        float Range() const { return range_; }
        const DirectX::XMFLOAT3& Position() const { return position_; }
        float Spot() const { return spot_; }
        const DirectX::XMFLOAT3& Direction() const { return direction_; }
        const DirectX::XMFLOAT3& Attenuation() const { return attenuation_; }

    private:
        DirectX::XMFLOAT3 strength_;
        float range_;
        DirectX::XMFLOAT3 position_;
        float spot_;
        DirectX::XMFLOAT3 direction_;
        float pad1;
        DirectX::XMFLOAT3 attenuation_;
        float pad2;
    };

    class MaterialsBuffer;
    class Material : public Utility::BufferEntryDirty
    {
        friend MaterialsBuffer;
    public:
        Material(const std::string& name, uint32 bufferIndex) :
            BufferEntryDirty(bufferIndex),
            name_(name)
        {}

        enum class Type {
            kRed, kGreen, kBlue, kWhite,
            kTurquesa,
            kEmerald,
            kJade,
            kObsidian,
            kSilver,
        };
        static std::unique_ptr<Material> CreatePredefined(Type type, const std::string& name, uint32 bufferIndex);

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
            SetAllFramesDirty();
        }

        const DirectX::XMFLOAT4& GetAmbient() const { return ambient_; }
        const DirectX::XMFLOAT4& GetDiffuse() const { return diffuse_; }
        const DirectX::XMFLOAT4& GetSpecular() const { return specular_; }
        float GetFresnelR0() const { return fresnelR0_; }
        float GetRoughness() const { return roughness_; }

    private:
        const std::string name_;

        DirectX::XMFLOAT4 ambient_;
        DirectX::XMFLOAT4 diffuse_;
        DirectX::XMFLOAT4 specular_;
        float fresnelR0_;
        float roughness_;
    };

    class LightsHolder
    {
    public:
        DirectionalLight* CreateDirLightBillet();
        PointLight* CreatePntLightBillet();
        SpotLight* CreateSptLightBillet();

        void DestroyLight(DirectionalLight* light) { DestroyLight<DirectionalLight, MAX_DIR_LIGHTS_COUNT>(dirLights_, light, dirLightsCount_); }
        void DestroyLight(PointLight* light) { DestroyLight<PointLight, MAX_PNT_LIGHTS_COUNT>(pntLights_, light, pntLightsCount_); }
        void DestroyLight(SpotLight* light) { DestroyLight<SpotLight, MAX_SPT_LIGHTS_COUNT>(sptLights_, light, sptLightsCount_); }

        auto DirLightsCount() const { return dirLightsCount_; }
        auto PntLightsCount() const { return pntLightsCount_; }
        auto SptLightsCount() const { return sptLightsCount_; }

        const auto& DirLights() const { return dirLights_; }
        const auto& PntLights() const { return pntLights_; }
        const auto& SptLights() const { return sptLights_; }

    private:
        template <class L, uint_t N>
        static void DestroyLight(std::array<std::unique_ptr<L>, N>& lights, L* light, uint8 lightsCount) {
            auto index = light->BufferIndex();
            lights[index].reset();
            // Move the last one into the empty slot
            if (lightsCount - 1 > (uint8)index) {
                lights[index] = move(lights[lightsCount - 1]);
                lights[index]->bufferIndex_ = index;
            }
        }

        std::array<std::unique_ptr<DirectionalLight>, MAX_DIR_LIGHTS_COUNT> dirLights_;
        std::array<std::unique_ptr<PointLight>, MAX_PNT_LIGHTS_COUNT> pntLights_;
        std::array<std::unique_ptr<SpotLight>, MAX_SPT_LIGHTS_COUNT> sptLights_;

        uint8 dirLightsCount_ = 0;
        uint8 pntLightsCount_ = 0;
        uint8 sptLightsCount_ = 0;
    };

    class MaterialsBufferIterator : public std::iterator<std::forward_iterator_tag, Material>
    {
    private:
        using Me = MaterialsBufferIterator;
        using Container = MaterialsBuffer;

        Container& container_;
        uint32 index_;

    public:
        explicit MaterialsBufferIterator(Container& c);
        MaterialsBufferIterator(Container& c, uint32 i) : container_(c), index_(i) {}

        bool operator==(const Me& other) { AssertCompatibility(other); return &container_ == &other.container_ && index_ == other.index_; }
        bool operator!=(const Me& other) { AssertCompatibility(other); return !(*this == other); }
        bool operator<(const Me& other) { AssertCompatibility(other); return index_ < other.index_; }
        bool operator>(const Me& other) { AssertCompatibility(other); return index_ > other.index_; }

        value_type& operator* ();

        Me& operator++();
        Me operator++(int);

    private:
        void AssertCompatibility(const Me& other) { if (&container_ != &other.container_) throw "Iterators are not compatible."; }
    };

    class MaterialsBuffer
    {
        friend MaterialsBufferIterator;
    public:
        MaterialsBuffer(uint32 materialsCountLimit);

        Material* Create(const std::string& name,
            const DirectX::XMFLOAT4& ambient,
            const DirectX::XMFLOAT4& diffuse,
            const DirectX::XMFLOAT4& specular,
            float fresnelR0,
            float roughness);
        Material* CreatePredefined(const std::string& name, Material::Type type);
        void Destroy(Material* material);

        MaterialsBufferIterator begin() { return MaterialsBufferIterator(*this); }
        MaterialsBufferIterator end()   { return MaterialsBufferIterator(*this, (uint32)materials_.size()); }

    private:
        std::unique_ptr<Utility::FreeIndices> freeIndices_;
        Common::Dynarray<std::unique_ptr<Material>> materials_;
    };
}
