#pragma once

#include "Pile\Attribute\NonCopyable.h"

namespace Viewer {
    class Viewport;
}

namespace Pipeline
{
    class UserMesh;
    class UserCollider;

    class UserScene : public Pile::NonCopyable
    {
    public:
        UserScene(Viewer::Viewport &viewport);
        ~UserScene();

        void AddMesh(UserMesh *mesh);
        const uint_t GetMeshesCount() const;
        const UserMesh& GetMesh(uint_t index) const;
        UserMesh& GetNonConstMesh(uint_t index);
        uint_t SearchMesh(const char* name) const;

        void AddCollider(UserCollider *collider);
        const uint_t GetCollidersCount() const;
        const UserCollider& GetCollider(uint_t index) const;
        UserCollider& GetNonConstCollider(uint_t index);
        uint_t SearchCollider(const char* name) const;

        Viewer::Viewport& GetViewport() { return viewport_; }
        
    private:
        std::vector<UserMesh*>     meshes_;
        std::vector<UserCollider*> colliders_;

        Viewer::Viewport& viewport_;
    };
}
