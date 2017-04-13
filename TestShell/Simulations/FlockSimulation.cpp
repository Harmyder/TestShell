#include "stdafx.h"
#include "Simulations\FlockSimulation.h"

#include "Viewer\Viewport.h"
#include "Viewer\Raii.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Harmyder\Interface\HarmyderInterface.h"
#include "Simulations\Utility.h"
#include "Common\Math\Vector\all.h"

using namespace Pipeline;
using namespace Viewer;
using namespace std;
using namespace Common;

FlockSimulation::FlockSimulation(Viewport& viewport, const GameInput& gameInput) : BaseSimulation("FlockSimulation", viewport, gameInput) {}
FlockSimulation::~FlockSimulation() {}

void FlockSimulation::Init()
{
    mats_[0] = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kEmerald(), "emerald"));
    mats_[1] = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kJade(), "jade"));
    mats_[2] = make_unique<MaterialRaii>(viewport_.CreateMaterial(MaterialType::kObsidian(), "obsidian"));

    ImportScene("..\\..\\FBX\\", "shark");

    const auto& mesh = scene_->GetMesh(0);
    const auto& mg = mesh.GetGeometry();
    const auto verticesCount = mg.UniquePositions.size();
    auto positions = make_unique<htPosition[]>(verticesCount);
    for (uint_t i = 0; i < verticesCount; ++i) {
        positions[i] = *(htPosition*)&mg.UniquePositions[i];
    }
    auto pointCloud = hfPointCloudRigidCreate(positions.get(), (uint32)verticesCount);
    auto sphere = hfComputeSphereBV(pointCloud);

    htFlockEntity entity{ pointCloud, sphere, {10, 5, 5}, 1.f };
    auto flockHandle = hfFlockCreate("Flock", entity);
    
    const uint32 instancesCount = hfFlockGetPiecesCount(flockHandle);
    auto pieces = hfFlockGetPieces(flockHandle);
    auto instancesDescs = make_unique<RenderItemInstanceDesc[]>(instancesCount);
    for (uint32 i = 0; i < instancesCount; ++i) {
        instancesDescs[i].transform = ((Matrix4*)&pieces[i])->Store4x3();
        instancesDescs[i].material = *mats_[i % 3];
    }

    RenderItemWithInstancesDesc desc("Flock",
        (const uint8*)mg.UniqueVertices.data(), (uint32)verticesCount,
        (const uint8*)mg.TrianglesVertices.data(), (uint32)mg.TrianglesVertices.size(), Texture(),
        Matrix4(kIdentity).Store4x3(),
        PrimitiveTopology::kTriangleList(),
        move(instancesDescs), instancesCount);
    flock_ = make_unique<RenderItemWithInstancesRaii>(viewport_.CreateRenderItemOpaqueWithInstances(desc, sizeof(VertexNormalTex)));
}

void FlockSimulation::Step(float deltaTime) {
    deltaTime;
}

