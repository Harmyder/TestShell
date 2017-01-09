#include "stdafx.h"
#include "Interface\GraphicsConsts.h"

namespace grePrimitiveTopology {
    extern const Type kInvalid((uint32)-1);
    extern const Type kTriangleList(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    extern const Type kLineList(D3D10_PRIMITIVE_TOPOLOGY_LINELIST);
}

namespace grePrimitiveTopologyType {
    extern const Type kInvalid((uint32)-1);
    extern const Type kTriangle(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
    extern const Type kLine(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
}

namespace greFillMode {
    extern const Type kInvalid;
    extern const Type kWireframe(D3D12_FILL_MODE_WIREFRAME);
    extern const Type kSolid(D3D12_FILL_MODE_SOLID);
};
