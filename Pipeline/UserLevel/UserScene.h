#pragma once

#include "Common\Attribute\NonCopyable.h"

namespace Pipeline
{
    class UserMesh;
    class UserCollider;

    class UserScene : public Common::NonCopyable
    {
    public:
        UserScene();
        ~UserScene();

        void AddMesh(std::unique_ptr<UserMesh> mesh);
        const uint_t GetMeshesCount() const { return meshes_.size(); }
        const UserMesh& GetMesh(uint_t index) const { return *meshes_[index]; }
        UserMesh& GetMeshNonConst(uint_t index) { return *meshes_[index]; }
        uint_t SearchMesh(const std::string& name) const;

        void AddCollider(std::unique_ptr<UserCollider> collider);
        const uint_t GetCollidersCount() const { return colliders_.size(); }
        const UserCollider& GetCollider(uint_t index) const { return *colliders_[index]; }
        UserCollider& GetNonConstCollider(uint_t index) { return *colliders_[index]; }
        uint_t SearchCollider(const char* name) const;

    private:
        std::vector<std::unique_ptr<UserMesh>>     meshes_;
        std::vector<std::unique_ptr<UserCollider>> colliders_;
    };
}
