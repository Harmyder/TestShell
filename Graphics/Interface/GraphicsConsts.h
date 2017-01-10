#pragma once

#include "Pile\DefineNamespaceEnumType.h"

namespace grePrimitiveTopology {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kTriangleList;
    extern const Type kLineList;
}

namespace grePrimitiveTopologyType {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kTriangle;
    extern const Type kLine;
}

namespace greFillMode {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kWireframe;
    extern const Type kSolid;
}

namespace greLibraryMaterial {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kRed;
    extern const Type kGreen;
    extern const Type kBlue;
    extern const Type kTurquesa;
    extern const Type kEmerald;
    extern const Type kJade;
    extern const Type kObsidian;
    extern const Type kSilver;
}
