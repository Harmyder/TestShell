#pragma once

#include "Common\DefineNamespaceEnumType.h"

namespace greVertexType {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kNormalTex;
    extern const Type kColor;
    extern const Type kParticles;
};

namespace greShaderType {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kLighting;
    extern const Type kLightingWithInstances;
    extern const Type kColor;
    extern const Type kParticles;
};

namespace greRootSignature {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kLighting;
    extern const Type kLightingWithInstances;
    extern const Type kColor;
    extern const Type kParticles;
};

namespace grePrimitiveTopology {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kTriangleList;
    extern const Type kLineList;
    extern const Type kPointList;
}

namespace grePrimitiveTopologyType {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kTriangle;
    extern const Type kLine;
    extern const Type kPoint;
}

namespace greFillMode {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kWireframe;
    extern const Type kSolid;
}

namespace greCullMode {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kNone;
    extern const Type kFront;
    extern const Type kBack;
}

namespace greLibraryMaterial {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kRed;
    extern const Type kGreen;
    extern const Type kBlue;
    extern const Type kWhite;
    extern const Type kTurquesa;
    extern const Type kEmerald;
    extern const Type kJade;
    extern const Type kObsidian;
    extern const Type kSilver;
}

namespace greResourceFormat {
    DEFINE_NAMESPACE_ENUM_TYPE(int, -1);
    extern const Type kInvalid;
    extern const Type kR8G8B8A8_UNORM;
}