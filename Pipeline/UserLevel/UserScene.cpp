#include "stdafx.h"

#include "UserLevel\UserScene.h"
#include "UserLevel\UserMesh.h"
#include "UserLevel\UserCollider.h"
#include "Viewer\Viewport.h"

using namespace std;

namespace Pipeline
{
    UserScene::UserScene(Viewer::Viewport &viewport) : viewport_(viewport) {}

    UserScene::~UserScene()
    {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i)
        {
            delete meshes_[i];
        }
    }

    void UserScene::AddMesh(UserMesh *mesh)
    {
        meshes_.push_back(mesh);
    }

    const uint_t UserScene::GetMeshesCount() const
    {
        return meshes_.size();
    }

    const UserMesh& UserScene::GetMesh(uint_t index) const
    {
        return *meshes_[index];
    }

    UserMesh& UserScene::GetNonConstMesh(uint_t index)
    {
        return *meshes_[index];
    }

    uint_t UserScene::SearchMesh(const string& name) const
    {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i)
        {
            if (name == meshes_[i]->GetName()) {
                return i;
            }
        }
        return Pile::NOT_FOUND;
    }

    void UserScene::AddCollider(UserCollider *collider)
    {
        colliders_.push_back(collider);
    }

    const uint_t UserScene::GetCollidersCount() const
    {
        return colliders_.size();
    }

    const UserCollider& UserScene::GetCollider(uint_t index) const
    {
        return *colliders_[index];
    }

    UserCollider& UserScene::GetNonConstCollider(uint_t index)
    {
        return *colliders_[index];
    }

    uint_t UserScene::SearchCollider(const char* name) const
    {
        const uint_t collidersCount = colliders_.size();
        for (uint_t i = 0; i < collidersCount; ++i)
        {
            if (strcmp(colliders_[i]->GetName().c_str(), name) == 0)
            {
                return i;
            }
        }
        return Pile::NOT_FOUND;
    }
}
