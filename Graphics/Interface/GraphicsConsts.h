#pragma once

#include "Pile\DefineEnumType.h"

namespace grePrimitiveTopology {
    DEFINE_ENUM_TYPE((uint32)-1);
    extern const Type kInvalid;
    extern const Type kTriangleList;
    extern const Type kLineList;
}

namespace grePrimitiveTopologyType {
    DEFINE_ENUM_TYPE((uint32)-1);
    extern const Type kInvalid;
    extern const Type kTriangle;
    extern const Type kLine;
}

namespace greFillMode {
    DEFINE_ENUM_TYPE((uint32)-1);
    extern const Type kInvalid;
    extern const Type kWireframe;
    extern const Type kSolid;
}
