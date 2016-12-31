// **************************************************************************************
// File: UserSceneFactory.cpp
// **************************************************************************************

#include "stdafx.h"

#include "UserLevel/UserSceneFactory.h"

#include "InputLevel\InputMesh.h"
#include "InputLevel\InputCollider.h"
#include "InputLevel\InputScene.h"
#include "UserLevel\UserMesh.h"
#include "UserLevel\UserCollider.h"
#include "UserLevel\UserScene.h"
#include "UserLevel\UserMeshFromInputMesh.h"
#include "Viewer\Viewport.h"

#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

namespace Pipeline
{
    static vector<Viewer::Viewport::RenderItemVerticesDesc> BuildMeshes(UserScene &userScene, const InputScene &inputScene) {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        for (uint_t i = 0; i < meshesCount; ++i) {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            UserMesh *mesh = new UserMesh(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore();
            userScene.AddMesh(mesh);
        }

        vector<Viewer::Viewport::RenderItemVerticesDesc> descs;
        descs.reserve(meshesCount);
        for (uint_t i = 0; i < meshesCount; ++i) {
            const auto& mesh = userScene.GetMesh(i);
            const auto& mg = mesh.GetGeometry();

            descs.emplace_back(mesh.GetName(), mg.Vertices, mesh.GetTransform());
        }
        return descs;
    }

    static vector<Viewer::Viewport::RenderItemTypeDesc> BuildColliders(UserScene &userScene, const InputScene &inputScene) {
        const uint_t collidersCount = inputScene.GetCollidersCount();
        for (uint_t i = 0; i < collidersCount; ++i) {
            const InputCollider &inputCollider = inputScene.GetCollider(i);

            UserCollider* collider = nullptr;
            switch (inputCollider.GetType()) {
            case ColliderType::kBox:
            {
                auto ibc = ((InputBoxCollider*)&inputCollider);
                collider = new UserColliderBox(inputCollider, ibc->GetExtents());
            }
                break;
            case ColliderType::kSphere:
            {
                auto isc = ((InputSphereCollider*)&inputCollider);
                collider = new UserColliderSphere(inputCollider, isc->GetRadius());
            }
                break;
            case ColliderType::kCapsule:
                throw;
                break;
            }
            collider->SetTransform(inputCollider.GetTransform());
            userScene.AddCollider(collider);
        }

        vector<Viewer::Viewport::RenderItemTypeDesc> descs;
        descs.reserve(collidersCount);
        for (uint_t i = 0; i < collidersCount; ++i) {
            const auto& collider = userScene.GetCollider(i);

            using namespace Viewer;
            PredefinedGeometryType type = PredefinedGeometryType::kSize;
            switch (collider.GetType()) {
            case ColliderType::kBox:
                type = PredefinedGeometryType::kBox;
                break;
            case ColliderType::kSphere:
                type = PredefinedGeometryType::kSphere;
                break;
            }

            XMMATRIX transform = XMLoadFloat4x4(&collider.GetTransform());
            transform = XMLoadFloat4x4(&collider.GetScalingTransform()) * transform;
            XMFLOAT4X4 t;
            XMStoreFloat4x4(&t, transform);
            descs.emplace_back(collider.GetName(), type, t);
        }

        return descs;
    }

    void UserSceneFactory::Build(UserScene &userScene, const InputScene &inputScene)
    {
        const auto vd = BuildMeshes(userScene, inputScene);
        const auto td = BuildColliders(userScene, inputScene);

        userScene.GetViewport().CreateRenderItem(vd, td);
    }
}
