#include "stdafx.h"
#include "Simulations/Cloth/ClothSimulation.h"

#include <vector>
using namespace std;

#include "Common/Math/Vector/all.h"
#include "Common/Geometry/GeometryGenerator.h"
using namespace Common;

#include "Pipeline/InputLevel/InputMesh.h"
#include "Pipeline/InputLevel/InputScene.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Pipeline/Helpers/CreateInputMesh.h"
using namespace Pipeline;

#include "Viewer/Vertex.h"
#include "Viewer/Raii.h"
using namespace Viewer;

#include "Simulations\Utility.h"

ClothSimulation::ClothSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("Clothsimulation", viewport, gameInput) {}
ClothSimulation::~ClothSimulation() {}

void ClothSimulation::Init() {
    auto inputMesh = make_unique<InputMesh>("cloth");
    Helpers::CreateGridXY(25, 25, .5f, .5f, *inputMesh);
    inputScene_ = make_unique<InputScene>();
    inputScene_->AddMesh(move(inputMesh));

    scene_ = make_unique<UserScene>();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene_);
    clothMesh_ = &scene_->GetMeshNonConst(scene_->SearchMesh("cloth"));
    InitBlankPhysicsData();

    matDeformable_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kTurquesa(), "rigid"));
    auto descs = BuildDescsFromScene(*scene_, *matDeformable_, *matDeformable_);

    cloth_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
}

void ClothSimulation::Step(float deltaTime) {
    deltaTime;
 
    auto& mg = clothMesh_->GetGeometryNonConst();
    for (auto& v : mg.UniqueVertices) {
        v.Position.z -= 0.1f;
    }
    viewport_.UpdateRenderSubItemVertexData(*cloth_, "cloth", (uint8*)mg.UniqueVertices.data());
}
