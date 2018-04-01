#include "stdafx.h"
#include "Simulations\CubeSimulation.h"

#include "Viewer\Viewport.h"
#include "Viewer\Raii.h"
#include "Common\Math\Vector\all.h"

using namespace Viewer;
using namespace std;
using namespace Common;

CONS_DEF(CubeSimulation);
CubeSimulation::~CubeSimulation() {
}

void CubeSimulation::Init() {
    matCollider_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kSilver(), "collider"));

    const RenderItemTypeDesc d("TheCube", PredefinedGeometryType::kBox, Matrix4(kIdentity).Store4x3(), *matCollider_, PrimitiveTopology::kTriangleList());

    cube_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque({d}));
}

void CubeSimulation::Step(float deltaTime) {
    deltaTime;
}
