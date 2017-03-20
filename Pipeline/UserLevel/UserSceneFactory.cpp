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

#include <DirectXMath.h>

using namespace std;
using namespace DirectX;

namespace Pipeline
{
    void UserSceneFactory::BuildMeshes(UserScene &userScene, const InputScene &inputScene) {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        for (uint_t i = 0; i < meshesCount; ++i) {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            auto mesh = make_unique<UserMesh>(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore();
            userScene.AddMesh(move(mesh));
        }
    }

    void UserSceneFactory::BuildColliders(UserScene &userScene, const InputScene &inputScene) {
        const uint_t collidersCount = inputScene.GetCollidersCount();
        for (uint_t i = 0; i < collidersCount; ++i) {
            const InputCollider &inputCollider = inputScene.GetCollider(i);

            unique_ptr<UserCollider> collider;
            switch (inputCollider.GetType()) {
            case ColliderType::kBox:
            {
                auto ibc = ((InputBoxCollider*)&inputCollider);
                collider = make_unique<UserColliderBox>(inputCollider, ibc->GetExtents());
            }
            break;
            case ColliderType::kSphere:
            {
                auto isc = ((InputSphereCollider*)&inputCollider);
                collider = make_unique<UserColliderSphere>(inputCollider, isc->GetRadius());
            }
            break;
            case ColliderType::kCapsule:
                throw;
                break;
            }
            collider->SetTransform(inputCollider.GetTransform());
            userScene.AddCollider(move(collider));
        }

    }

    void UserSceneFactory::BuildScene(UserScene &userScene, const InputScene &inputScene) {
        BuildMeshes(userScene, inputScene);
        BuildColliders(userScene, inputScene);
    }
}
