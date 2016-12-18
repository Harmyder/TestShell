// **************************************************************************************
// Desc: Stores whole imported scene data
// **************************************************************************************
#pragma once

namespace Pipeline
{
    class InputMesh;

    class InputScene
    {
    public:
        ~InputScene();

        void AddMesh(InputMesh *mesh);
        const uint_t GetMeshesCount() const;
        const InputMesh& GetMesh(uint_t index) const;
        uint_t SearchMesh(const char* name) const;

    private:
        std::vector<InputMesh*> meshes_;
    };
}