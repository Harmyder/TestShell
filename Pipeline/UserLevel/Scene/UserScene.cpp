#include "stdafx.h"

#include "UserLevel/Scene/UserScene.h"
#include "UserLevel/Mesh/UserMesh.h"
#include "UserLevel/Collider/UserCollider.h"

namespace Pipeline
{
    // **************************************************************************************
    UserScene::~UserScene()
    {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i)
        {
            delete meshes_[i];
        }
    }

    // **************************************************************************************
    void UserScene::AddMesh(UserMesh *mesh)
    {
        meshes_.push_back(mesh);
    }

    // **************************************************************************************
    const uint_t UserScene::GetMeshesCount() const
    {
        return meshes_.size();
    }

    // **************************************************************************************
    const UserMesh& UserScene::GetMesh(uint32 index) const
    {
        return *meshes_[index];
    }

    // **************************************************************************************
    UserMesh& UserScene::GetNonConstMesh(uint32 index)
    {
        return *meshes_[index];
    }

    // **************************************************************************************
    uint_t UserScene::SearchMesh(const char* name) const
    {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i)
        {
            if (strcmp(meshes_[i]->GetName(), name) == 0)
            {
                return i;
            }
        }
        return Pile::NOT_FOUND;
    }

    // **************************************************************************************
    void UserScene::AddCollider(UserCollider *collider)
    {
        colliders_.push_back(collider);
    }

    // **************************************************************************************
    const uint32 UserScene::GetCollidersCount() const
    {
        return colliders_.size();
    }

    // **************************************************************************************
    const UserCollider& UserScene::GetCollider(uint32 index) const
    {
        return *colliders_[index];
    }

    // **************************************************************************************
    UserCollider& UserScene::GetNonConstCollider(uint32 index)
    {
        return *colliders_[index];
    }

    // **************************************************************************************
    uint32 UserScene::SearchCollider(const char* name) const
    {
        const uint_t collidersCount = colliders_.size();
        for (uint_t i = 0; i < collidersCount; ++i)
        {
            if (strcmp(colliders_[i]->GetName(), name) == 0)
            {
                return i;
            }
        }
        return Pile::NOT_FOUND;
    }
}
