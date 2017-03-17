#include "stdafx.h"
#include "Simulations\FlockSimulation.h"

#include "Viewer\Viewport.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Harmyder\Interface\HarmyderInterface.h"
#include "Simulations\Utility.h"
#include "Common\Math\Transform.h"

using namespace Pipeline;
using namespace Viewer;
using namespace std;
using namespace Common;

FlockSimulation::FlockSimulation(Viewport& viewport, const GameInput& gameInput) : BaseSimulation("FlockSimulation", viewport, gameInput) {}
FlockSimulation::~FlockSimulation() {}

void FlockSimulation::Init()
{
    viewport_.CreateMaterial(Material::kEmerald(), "emerald");
    viewport_.CreateMaterial(Material::kJade(), "jade");
    viewport_.CreateMaterial(Material::kObsidian(), "obsidian");

    const string path = "..\\..\\FBX\\";
    const string filetitle = "shark";
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    factory.BuildScene(*scene_, *inputScene);

    const auto& mesh = scene_->GetMesh(0);
    const auto& mg = mesh.GetGeometry();
    const auto verticesCount = mg.UniquePositions.size();
    auto positions = make_unique<htPosition[]>(verticesCount);
    for (uint_t i = 0; i < verticesCount; ++i) {
        positions[i] = *(htPosition*)&mg.UniquePositions[i];
    }
    auto pointCloud = hfPointCloudRigidCreate(positions.get(), (uint32)verticesCount);
    auto sphere = hfComputeSphereBV(pointCloud);

    htFlockEntity entity{ pointCloud, &sphere, {10, 5, 5}, 1.f };
    auto flockHandle = hfFlockCreate("Flock", entity);
    
    const uint32 instancesCount = hfFlockGetPiecesCount(flockHandle);
    auto pieces = hfFlockGetPieces(flockHandle);
    vector<RenderItemInstanceDesc> instancesDescs;
    instancesDescs.reserve(instancesCount);
    string mats[3] = { "emerald", "jade", "obsidian" };
    for (uint32 i = 0; i < instancesCount; ++i) {
        instancesDescs.emplace_back(*(XMFLOAT4X3*)&pieces[i], mats[i % 3]);
    }

    RenderItemWithInstancesDesc desc("Flock",
        (const uint8*)mg.UniqueVertices.data(), (uint32)verticesCount,
        AffineTransform(kIdentity).Store(),
        PrimitiveTopology::kTriangleList(),
        instancesDescs.data(), instancesCount);
    flock_ = make_unique<StructRenderItemWithInstancesId>(viewport_.CreateRenderItemOpaqueWithInstances(desc, sizeof(VertexNormalTex)));
}

void FlockSimulation::Step(float deltaTime)
{
    deltaTime;
}

void FlockSimulation::Quit()
{
    if (flock_) viewport_.DestroyRenderItemOpaqueWithInstances(*flock_);
    viewport_.DestroyMaterial("emerald");
    viewport_.DestroyMaterial("jade");
    viewport_.DestroyMaterial("obsidian");
}
