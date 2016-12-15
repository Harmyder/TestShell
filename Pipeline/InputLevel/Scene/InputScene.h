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
        const uint32 GetMeshesCount() const;
        const InputMesh& GetMesh(uint32 index) const;
        uint32 SearchMesh(const char* name) const;

    private:
        std::vector<InputMesh*> meshes_;
    };
}