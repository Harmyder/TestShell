#include "stdafx.h"

#include "Simulations\FbxSimulation.h"
#include "Viewer\Viewport.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Harmyder\Interface\HarmyderInterface.h"

using namespace Viewer;
using namespace std;
using namespace Pipeline;

FbxSimulation::FbxSimulation(Viewer::Viewport& viewport, const Viewer::GameInput& gameInput) :
    BaseSimulation("FbxSimulation", viewport, gameInput) {}
FbxSimulation::~FbxSimulation() {}

void FbxSimulation::Init() {
    viewport_.CreateMaterial(Material::kEmerald(), "rigid");
    viewport_.CreateMaterial(Material::kSilver(), "collider");

    const string path = "..\\..\\FBX\\";
    const string filetitle = "teapot001";
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    RenderItemsDescriptions descs;
    descs = factory.Build(*scene_, *inputScene);
    if (descs.Vertices.size() > 0) sceneDescsVertices_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
    if (descs.Types.size() > 0) sceneDescsTypes_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Types));
}

void FbxSimulation::Step(float deltaTime) {
    deltaTime;
}

void FbxSimulation::Quit() {
    if (sceneDescsVertices_) viewport_.DestroyRenderItemOpaque(*sceneDescsVertices_);
    if (sceneDescsTypes_) viewport_.DestroyRenderItemOpaque(*sceneDescsTypes_);
}