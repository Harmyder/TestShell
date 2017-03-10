#include "stdafx.h"
#include "Simulations\CubeSimulation.h"

#include "Viewer\Viewport.h"
#include "Pile\Math\XmFloatHelper.h"

using namespace Viewer;
using namespace std;

CubeSimulation::CubeSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("CubeSimulation", viewport, gameInput) {}
CubeSimulation::~CubeSimulation() {}

void CubeSimulation::Init() {
    viewport_.CreateMaterial(Material::kSilver(), "collider");

    const RenderItemTypeDesc d("TheCube", PredefinedGeometryType::kBox, Pile::Identity4x4(), "collider", PrimitiveTopology::kTriangleList());

    cube_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque({d}));
}

void CubeSimulation::Step(float deltaTime) {
    deltaTime;
}

void CubeSimulation::Quit() {
    viewport_.DestroyRenderItemOpaque(*cube_);
    viewport_.DestroyMaterial("collider");
}