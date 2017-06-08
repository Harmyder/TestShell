#include "stdafx.h"

#include "Simulations\FbxSimulation.h"
#include "Viewer\Viewport.h"
#include "Viewer\Raii.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Harmyder\Interface\HarmyderInterface.h"
#include "Simulations\Utility.h"

using namespace Viewer;
using namespace std;
using namespace Pipeline;

CONS_DEF(FbxSimulation);
FbxSimulation::~FbxSimulation() {}

void FbxSimulation::Init() {
    matRigid_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kEmerald(), "rigid"));
    matCollider_ = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kSilver(), "collider"));

    const string path = "..\\..\\Assets\\FBX\\";
    const string filetitle = "teapot001";
    ImportScene(path, filetitle);
    InitBlankPhysicsData();
    auto descs = BuildDescsFromScene(*scene_, *matRigid_, *matCollider_);

    if (descs.Vertices.size() > 0) sceneDescsVertices_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
    if (descs.Types.size() > 0) sceneDescsTypes_ = make_unique<RenderItemOpaqueRaii>(viewport_.CreateRenderItemOpaque(descs.Types));
}

void FbxSimulation::Step(float deltaTime) {
    deltaTime;
}
