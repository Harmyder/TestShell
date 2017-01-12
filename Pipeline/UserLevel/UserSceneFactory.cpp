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
using namespace Viewer;

namespace Pipeline
{
    static DescsVertices BuildMeshes(UserScene &userScene, const InputScene &inputScene) {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        for (uint_t i = 0; i < meshesCount; ++i) {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            UserMesh *mesh = new UserMesh(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore();
            userScene.AddMesh(mesh);
        }

        DescsVertices descs;
        descs.reserve(meshesCount);
        for (uint_t i = 0; i < meshesCount; ++i) {
            const auto& mesh = userScene.GetMesh(i);
            const auto& mg = mesh.GetGeometry();

            RenderItemVerticesDesc desc(mesh.GetName(), (uint8*)mg.Vertices.data(), (uint32)mg.Vertices.size(), mesh.GetTransform(), "rigid", PrimitiveTopology::kTriangleList());
            descs.push_back(desc);
        }
        return descs;
    }

    static DescsTypes BuildColliders(UserScene &userScene, const InputScene &inputScene) {
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

        DescsTypes descs;
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

            XMMATRIX transform = XMLoadFloat4x4(&collider.GetTransform( ));
            transform = XMLoadFloat4x4(&collider.GetScalingTransform()) * transform;
            XMFLOAT4X4 t; XMStoreFloat4x4(&t, transform);
            RenderItemTypeDesc d(collider.GetName(), type, t, "collider", PrimitiveTopology::kTriangleList());
            descs.push_back(d);
        }

        return descs;
    }

    RenderItemsDescriptions UserSceneFactory::Build(UserScene &userScene, const InputScene &inputScene)
    {
        return { move(BuildMeshes(userScene, inputScene)), move(BuildColliders(userScene, inputScene)) };
    }
}
