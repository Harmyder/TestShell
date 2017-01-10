#include "stdafx.h"
#include "Core\Lighting.h"
#include "Interface\GraphicsConsts.h"

using namespace Graphics;

namespace grePrimitiveTopology {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_PRIMITIVE_TOPOLOGY>>::value,
        "Underlying type of D3D12_PRIMITIVE_TOPOLOGY is different from grePrimitiveTopology::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kTriangleList(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    extern const Type kLineList(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
}

namespace grePrimitiveTopologyType {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_PRIMITIVE_TOPOLOGY_TYPE>>::value,
        "Underlying type of D3D12_PRIMITIVE_TOPOLOGY_TYPE is different from grePrimitiveTopologyType::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kTriangle(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    extern const Type kLine(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}

namespace greFillMode {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<D3D12_FILL_MODE>>::value,
        "Underlying type of D3D12_FILL_MODE is different from greFillMode::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kWireframe(D3D12_FILL_MODE_WIREFRAME);
    extern const Type kSolid(D3D12_FILL_MODE_SOLID);
}

namespace greLibraryMaterial {
    static_assert(std::is_same<Type::UnderlyingType, std::underlying_type_t<Material::Type>>::value, 
        "Underlying type of Graphics::Material is different from greLibraryMaterial::UnderlyingType");
    extern const Type kInvalid = Type();
    extern const Type kRed     ((Type::UnderlyingType)Material::Type::kRed);
    extern const Type kGreen   ((Type::UnderlyingType)Material::Type::kGreen);
    extern const Type kBlue    ((Type::UnderlyingType)Material::Type::kBlue);
    extern const Type kTurquesa((Type::UnderlyingType)Material::Type::kTurquesa);
    extern const Type kEmerald ((Type::UnderlyingType)Material::Type::kEmerald);
    extern const Type kJade    ((Type::UnderlyingType)Material::Type::kJade);
    extern const Type kObsidian((Type::UnderlyingType)Material::Type::kObsidian);
    extern const Type kSilver  ((Type::UnderlyingType)Material::Type::kSilver);
}
