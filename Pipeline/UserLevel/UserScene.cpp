#include "stdafx.h"

#include "UserLevel\UserScene.h"
#include "UserLevel\UserMesh.h"
#include "UserLevel\UserCollider.h"

using namespace std;

namespace Pipeline
{
    UserScene::UserScene() {}
    UserScene::~UserScene() {}

    void UserScene::AddMesh(std::unique_ptr<UserMesh> mesh) { meshes_.push_back(move(mesh)); }

    void UserScene::AddCollider(std::unique_ptr<UserCollider> collider) { colliders_.push_back(move(collider)); }

    uint_t UserScene::SearchMesh(const std::string& name) const {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i) {
            if (name == meshes_[i]->GetName()) return i;
        }
        return Common::NOT_FOUND;
    }

    uint_t UserScene::SearchCollider(const char* name) const {
        const uint_t collidersCount = colliders_.size();
        for (uint_t i = 0; i < collidersCount; ++i) {
            if (strcmp(colliders_[i]->GetName().c_str(), name) == 0) return i;
        }
        return Common::NOT_FOUND;
    }
}
