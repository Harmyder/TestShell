#include "stdafx.h"
#include "Simulations/SphereBvSimulation.h"

#include "Viewer/Viewport.h"
#include "Pipeline/UserLevel/UserScene.h"
#include "Pipeline/UserLevel/UserMesh.h"
#include "Pipeline/SceneManager/SceneManager.h"
#include "Pipeline/UserLevel/UserSceneFactory.h"
#include "Harmyder/Interface/HarmyderInterface.h"
#include "Simulations/Utility.h"
#include "Common/Math/Vector/Transform.h"

using namespace Pipeline;
using namespace Viewer;
using namespace std;
using namespace DirectX;
using namespace Common;

SphereBvSimulation::SphereBvSimulation(Viewport& viewport, const GameInput& gameInput) : BaseSimulation("SphereBvSimulation", viewport, gameInput) {}
SphereBvSimulation::~SphereBvSimulation() {}

void SphereBvSimulation::Init() {
    viewport_.CreateMaterial(Material::kEmerald(), "rigid");
    viewport_.CreateMaterial(Material::kJade(), "collider");
    viewport_.CreateMaterial(Material::kSilver(), "boundingVolume");

    const string path = "..\\..\\FBX\\";
    const string filetitle = "shark";
    ImportScene(path, filetitle);
    InitBlankPhysicsData();
    auto descs = BuildDescsFromScene(*scene_);
    if (descs.Vertices.size() > 0) sceneDescsVertices_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Vertices, sizeof(VertexNormalTex)));
    if (descs.Types.size() > 0) sceneDescsTypes_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemOpaque(descs.Types));

    const auto& mesh = scene_->GetMesh(0);
    
    const auto& mg = mesh.GetGeometry();
    const auto verticesCount = mg.UniquePositions.size();
    unique_ptr<htPosition[]> positions(new htPosition[verticesCount]);
    for (uint_t i = 0; i < verticesCount; ++i) {
        positions[i] = *(htPosition*)&mg.UniquePositions[i];
    }
    auto pointCloud = hfPointCloudRigidCreate(positions.get(), (uint32)verticesCount);
    auto sphere = hfComputeSphereBV(pointCloud);

    auto rigidBody = hfRigidBodyCreate(pointCloud, sphere);
    hfRigidBodySetTransform(rigidBody, *(htTransform*)&mesh.GetTransform());

    vector<RenderItemTypeDesc> descsBV;
    PredefinedGeometryType type = PredefinedGeometryType::kSphere;

    const auto translation = OrthogonalTransform::MakeTranslation(sphere.center.coordinates[0], sphere.center.coordinates[1], sphere.center.coordinates[2]);
    const auto meshT = mesh.GetTransform();
    const auto transform = translation * meshT;
    const auto scale = AffineTransform::MakeScale(sphere.radius);
    XMFLOAT4X3 t = (scale * OrthoToAffine(transform)).Store();
    descsBV.emplace_back("", type, t, "boundingVolume", PrimitiveTopology::kTriangleList());
    boundingVolumeDesc_ = make_unique<StructRenderItemId>(viewport_.CreateRenderItemTransparent(descsBV));
}

void SphereBvSimulation::Step(float deltaTime) {
    deltaTime;
}

void SphereBvSimulation::Quit() {
    if (sceneDescsVertices_) viewport_.DestroyRenderItemOpaque(*sceneDescsVertices_);
    if (sceneDescsTypes_) viewport_.DestroyRenderItemOpaque(*sceneDescsTypes_);
    if (boundingVolumeDesc_) viewport_.DestroyRenderItemTransparent(*boundingVolumeDesc_);
    viewport_.DestroyMaterial("rigid");
    viewport_.DestroyMaterial("collider");
    viewport_.DestroyMaterial("boundingVolume");
}