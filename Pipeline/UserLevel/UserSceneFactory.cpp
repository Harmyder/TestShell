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
    static void BuildMeshes(UserScene &userScene, const InputScene &inputScene) {
        const uint_t meshesCount = inputScene.GetMeshesCount();
        for (uint_t i = 0; i < meshesCount; ++i) {
            const InputMesh &inputMesh = inputScene.GetMesh(i);
            UserMesh *mesh = new UserMesh(inputMesh);
            UserMeshFromInputMesh restorer(*mesh);
            restorer.Restore();
            userScene.AddMesh(mesh);
        }
    }

    static void BuildColliders(UserScene &userScene, const InputScene &inputScene) {
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

    }

    void UserSceneFactory::BuildScene(UserScene &userScene, const InputScene &inputScene) {
        BuildMeshes(userScene, inputScene);
        BuildColliders(userScene, inputScene);
    }
}
