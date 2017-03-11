#pragma once

#include "Common\Attribute\NonCopyable.h"

namespace Pipeline
{
    class UserMesh;
    class UserCollider;

    class UserScene : public Common::NonCopyable
    {
    public:
        ~UserScene();

        void AddMesh(UserMesh *mesh);
        const uint_t GetMeshesCount() const;
        const UserMesh& GetMesh(uint_t index) const;
        UserMesh& GetMeshNonConst(uint_t index);
        uint_t SearchMesh(const std::string& name) const;

        void AddCollider(UserCollider *collider);
        const uint_t GetCollidersCount() const;
        const UserCollider& GetCollider(uint_t index) const;
        UserCollider& GetNonConstCollider(uint_t index);
        uint_t SearchCollider(const char* name) const;

    private:
        std::vector<UserMesh*>     meshes_;
        std::vector<UserCollider*> colliders_;
    };
}
