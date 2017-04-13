#include "stdafx.h"
#include "Simulations\Utility.h"

#include "Pipeline\UserLevel\UserScene.h"
#include "Pipeline\UserLevel\UserMesh.h"
#include "Pipeline\UserLevel\UserCollider.h"
using namespace Viewer;
using namespace std;

RenderItemWithInstancesDesc BuildDescInstancedFromMesh(const Pipeline::UserMesh& mesh, Material matRigid) {
    const auto& mg = mesh.GetGeometry();
    auto instances = make_unique<RenderItemInstanceDesc[]>(mesh.GetTransformsCount());
    for (uint32 i = 0; i < mesh.GetTransformsCount(); ++i) {
        instances[i].transform = mesh.GetTransform(i).Store4x3();
        instances[i].material = matRigid;
    }
    return RenderItemWithInstancesDesc(mesh.GetName(),
        (uint8*)mg.UniqueVertices.data(), (uint32)mg.UniqueVertices.size(),
        (uint8*)mg.TrianglesVertices.data(), (uint32)mg.TrianglesVertices.size(), Texture(),
        Common::Matrix4(Common::kIdentity).Store4x3(), PrimitiveTopology::kTriangleList(), move(instances), mesh.GetTransformsCount());
}

RenderItemVerticesDesc BuildDescFromMesh(const Pipeline::UserMesh& mesh, Material matRigid) {
    const auto& mg = mesh.GetGeometry();
    return RenderItemVerticesDesc(mesh.GetName(),
        (uint8*)mg.UniqueVertices.data(), (uint32)mg.UniqueVertices.size(),
        (uint8*)mg.TrianglesVertices.data(), (uint32)mg.TrianglesVertices.size(),
        mesh.GetTransform().Store4x3(), matRigid, Texture(), PrimitiveTopology::kTriangleList());
}

RenderItemTypeDesc BuildDescFromCollider(const Pipeline::UserCollider& collider, Material matCollider) {
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
    return RenderItemTypeDesc(collider.GetName(), type, t, matCollider, PrimitiveTopology::kTriangleList());
}

RenderItemsDescriptions BuildDescsFromScene(const Pipeline::UserScene& scene, Material matRigid, Material matCollider) {
    DescsVertices descsM;
    DescsInstanced descsI;
    const uint_t meshesCount = scene.GetMeshesCount();
    descsM.reserve(meshesCount);
    for (uint_t i = 0; i < meshesCount; ++i) {
        const auto& mesh = scene.GetMesh(i);
        if (mesh.GetTransformsCount() > 1) descsI.push_back(BuildDescInstancedFromMesh(mesh, matRigid));
        else if (mesh.GetTransformsCount() == 1) descsM.push_back(BuildDescFromMesh(mesh, matRigid));
        else assert(0 && "Mesh must have a transform or transforms at this point");
    }

    DescsTypes descsC;
    const uint_t collidersCount = scene.GetCollidersCount();
    descsC.reserve(collidersCount);
    for (uint_t i = 0; i < collidersCount; ++i) {
        const auto& collider = scene.GetCollider(i);
        descsC.push_back(BuildDescFromCollider(collider, matCollider));
    }

    return{ move(descsI), move(descsM), move(descsC) };
}