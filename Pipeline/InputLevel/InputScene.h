// **************************************************************************************
// Desc: Stores whole imported scene data
// **************************************************************************************
#pragma once

namespace Pipeline
{
    class InputMesh;
    class InputCollider;

    class InputScene
    {
    public:
        InputScene();
        ~InputScene();

        void AddMesh(std::unique_ptr<InputMesh> mesh);
        void AddCollider(std::unique_ptr<InputCollider> collider);

        const uint_t GetMeshesCount() const { return meshes_.size(); }
        const InputMesh& GetMesh(uint_t index) const { return *meshes_[index]; }

        const uint_t GetCollidersCount() const { return colliders_.size(); }
        const InputCollider& GetCollider(uint_t index) const { return *colliders_[index]; }

    private:
        std::vector<std::unique_ptr<InputMesh>> meshes_;
        std::vector<std::unique_ptr<InputCollider>> colliders_;
    };
}