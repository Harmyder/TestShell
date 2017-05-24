#include "stdafx.h"
#include "Core\Lighting.h"
#include "SDK\PipelineStateObject.h"
#include "SDK\RootSignature.h"
#include "Interface\GraphicsConsts.h"

using namespace Graphics;

namespace greVertexType {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<VertexType>>::value,
        "Underlying type of Graphics::VertexType is different from greVertexType::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kNormalTex((Type::UnderlyingType)VertexType::kNormalTex);
    extern const Type kColor    ((Type::UnderlyingType)VertexType::kColor);
    extern const Type kParticles((Type::UnderlyingType)VertexType::kParticles);
};

namespace greShaderType {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<ShaderType>>::value,
        "Underlying type of Graphics::ShaderType is different from greShaderType::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kLighting             ((Type::UnderlyingType)ShaderType::kLighting);
    extern const Type kLightingWithInstances((Type::UnderlyingType)ShaderType::kLightingWithInstances);
    extern const Type kColor                ((Type::UnderlyingType)ShaderType::kColor);
    extern const Type kParticles            ((Type::UnderlyingType)ShaderType::kParticles);
};

namespace greRootSignature {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<RootSignatureType>>::value,
        "Underlying type of Graphics::ShaderType is different from greRootSignature::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kLighting             ((Type::UnderlyingType)RootSignatureType::kLighting);
    extern const Type kLightingWithInstances((Type::UnderlyingType)RootSignatureType::kLightingWithInstances);
    extern const Type kColor                ((Type::UnderlyingType)RootSignatureType::kColor);
    extern const Type kParticles            ((Type::UnderlyingType)RootSignatureType::kParticles);
};

namespace grePrimitiveTopology {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_PRIMITIVE_TOPOLOGY>>::value,
        "Underlying type of D3D12_PRIMITIVE_TOPOLOGY is different from grePrimitiveTopology::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kTriangleList(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    extern const Type kLineList    (D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
}

namespace grePrimitiveTopologyType {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_PRIMITIVE_TOPOLOGY_TYPE>>::value,
        "Underlying type of D3D12_PRIMITIVE_TOPOLOGY_TYPE is different from grePrimitiveTopologyType::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kTriangle(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    extern const Type kLine    (D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
    extern const Type kPoint   (D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
}

namespace greFillMode {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_FILL_MODE>>::value,
        "Underlying type of D3D12_FILL_MODE is different from greFillMode::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kWireframe(D3D12_FILL_MODE_WIREFRAME);
    extern const Type kSolid    (D3D12_FILL_MODE_SOLID);
}

namespace greLibraryMaterial {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<Material::Type>>::value, 
        "Underlying type of Graphics::Material is different from greLibraryMaterial::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kRed     ((Type::UnderlyingType)Material::Type::kRed);
    extern const Type kGreen   ((Type::UnderlyingType)Material::Type::kGreen);
    extern const Type kBlue    ((Type::UnderlyingType)Material::Type::kBlue);
    extern const Type kWhite   ((Type::UnderlyingType)Material::Type::kWhite);
    extern const Type kTurquesa((Type::UnderlyingType)Material::Type::kTurquesa);
    extern const Type kEmerald ((Type::UnderlyingType)Material::Type::kEmerald);
    extern const Type kJade    ((Type::UnderlyingType)Material::Type::kJade);
    extern const Type kObsidian((Type::UnderlyingType)Material::Type::kObsidian);
    extern const Type kSilver  ((Type::UnderlyingType)Material::Type::kSilver);
}

namespace greResourceFormat {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<DXGI_FORMAT>>::value,
        "Underlying type of DXGI_FORMAT is different from greLibraryMaterial::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kR8G8B8A8_UNORM(DXGI_FORMAT_R8G8B8A8_UNORM);
}