#include "stdafx.h"
#include "Simulations\SphereBvSimulation.h"

#include "Viewer\Viewport.h"
#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\SceneManager\SceneManager.h"
#include "Pipeline\UserLevel\UserSceneFactory.h"
#include "Harmyder\Interface\HarmyderInterface.h"

using namespace Pipeline;
using namespace Viewer;
using namespace std;
using namespace DirectX;

void SphereBvSimulation::Init(Viewport &viewport)
{
    BaseSimulation::Init(viewport);

    viewport.CreateMaterial(Material::kEmerald(), "rigid");
    viewport.CreateMaterial(Material::kJade(), "collider");
    viewport.CreateMaterial(Material::kSilver(), "boundingVolume");

    const string path = "..\\..\\FBX\\";
    const string filetitle = "shark";
    SceneManager &manager = SceneManager::GetInstance();
    manager.Load(path, filetitle);

    InputScene *inputScene = manager.GetScene();
    UserSceneFactory factory;
    factory.Build(*scene_, *inputScene);

    const auto& mesh = scene_->GetMesh(0);
    
    const auto& mg = mesh.GetGeometry();
    const auto verticesCount = mg.Vertices.size();
    unique_ptr<htPosition[]> positions(new htPosition[verticesCount]);
    for (uint_t i = 0; i < verticesCount; ++i) {
        positions[i] = *(htPosition*)&mg.Vertices[i].Position;
    }
    auto pointCloud = hfCreatePointCloudRigid(positions.get(), (uint32)verticesCount);
    auto sphere = hfComputeSphereBV(pointCloud);

    auto rigidBody = hfCreateRigidBody(pointCloud, sphere);
    hfSetTransform(rigidBody, *(htTransform*)&mesh.GetTransform());

    vector<Viewer::Viewport::RenderItemTypeDesc> descs;
    PredefinedGeometryType type = PredefinedGeometryType::kSphere;

    XMMATRIX translation = XMMatrixTranslation(sphere.center[0], sphere.center[1], sphere.center[2]);
    XMMATRIX meshT = XMLoadFloat4x4(&mesh.GetTransform());
    XMMATRIX transform = XMMatrixMultiply(translation, meshT);
    XMMATRIX scale = XMMatrixScaling(sphere.radius, sphere.radius, sphere.radius);
    transform = XMMatrixMultiply(scale, transform);
    XMFLOAT4X4 t; XMStoreFloat4x4(&t, transform);
    descs.emplace_back("", type, t, "boundingVolume", PrimitiveTopology::kTriangleList());
    viewport_->CreateRenderItemTransparent(descs);
}

void SphereBvSimulation::Step(float deltaTime)
{
    deltaTime;
}

void SphereBvSimulation::Quit()
{
}