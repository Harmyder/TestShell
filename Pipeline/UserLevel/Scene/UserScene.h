#pragma once

#include "Pile\Attribute\NonCopyable.h"

namespace Pipeline
{
    class UserMesh;
    class UserCollider;

    class UserScene : public Pile::NonCopyable
    {
    public:
        ~UserScene();

        void AddMesh(UserMesh *mesh);
        const uint32 GetMeshesCount() const;
        const UserMesh& GetMesh(uint32 index) const;
        UserMesh& GetNonConstMesh(uint32 index);
        uint32 SearchMesh(const char* name) const;

        void AddCollider(UserCollider *collider);
        const uint32 GetCollidersCount() const;
        const UserCollider& GetCollider(uint32 index) const;
        UserCollider& GetNonConstCollider(uint32 index);
        uint32 SearchCollider(const char* name) const;

    private:
        std::vector<UserMesh*>     meshes_;
        std::vector<UserCollider*> colliders_;
    };
}
