#include "stdafx.h"
#include "Simulations\Utility.h"

#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\UserLevel\UserCollider.h"

Viewer::RenderItemVerticesDesc BuildDescFromMesh(const Pipeline::UserMesh& mesh) {
    const auto& mg = mesh.GetGeometry();
    using namespace Viewer;
    return RenderItemVerticesDesc(mesh.GetName(),
        (uint8*)mg.UniqueVertices.data(), (uint32)mg.UniqueVertices.size(),
        (uint8*)mg.TrianglesVertices.data(), (uint32)mg.TrianglesVertices.size(),
        mesh.GetTransform().Store(), "rigid", PrimitiveTopology::kTriangleList());
}

Viewer::RenderItemTypeDesc BuildDescFromCollider(const Pipeline::UserCollider& collider) {
    using namespace Viewer;
    PredefinedGeometryType type = PredefinedGeometryType::kSize;
    switch (collider.GetType()) {
    case Pipeline::ColliderType::kBox:
        type = PredefinedGeometryType::kBox;
        break;
    case Pipeline::ColliderType::kSphere:
        type = PredefinedGeometryType::kSphere;
        break;
    }

    XMMATRIX transform = XMLoadFloat4x4(&collider.GetTransform());
    transform = XMLoadFloat4x4(&collider.GetScalingTransform()) * transform;
    XMFLOAT4X3 t; XMStoreFloat4x3(&t, transform);
    return RenderItemTypeDesc(collider.GetName(), type, t, "collider", PrimitiveTopology::kTriangleList());
}

Viewer::RenderItemsDescriptions BuildDescsFromScene(const Pipeline::UserScene& scene) {
    using namespace Viewer;
    DescsVertices descsM;
    const uint_t meshesCount = scene.GetMeshesCount();
    descsM.reserve(meshesCount);
    for (uint_t i = 0; i < meshesCount; ++i) {
        const auto& mesh = scene.GetMesh(i);
        descsM.push_back(BuildDescFromMesh(mesh));
    }

    DescsTypes descsC;
    const uint_t collidersCount = scene.GetCollidersCount();
    descsC.reserve(collidersCount);
    for (uint_t i = 0; i < collidersCount; ++i) {
        const auto& collider = scene.GetCollider(i);
        descsC.push_back(BuildDescFromCollider(collider));
    }

    return{ move(descsM), move(descsC) };
}