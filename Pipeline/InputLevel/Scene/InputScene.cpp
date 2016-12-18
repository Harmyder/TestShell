#include "stdafx.h"

#include "InputLevel/Scene/InputScene.h"
#include "InputLevel/Mesh/InputMesh.h"

namespace Pipeline
{
    // **************************************************************************************
    InputScene::~InputScene()
    {
        const uint_t meshesCount = meshes_.size();
        for (uint_t i = 0; i < meshesCount; ++i)
        {
            delete meshes_[i];
        }
    }

    // **************************************************************************************
    void InputScene::AddMesh(InputMesh *mesh)
    {
        meshes_.push_back(mesh);
    }

    // **************************************************************************************
    const uint_t InputScene::GetMeshesCount() const
    {
        return meshes_.size();
    }

    // **************************************************************************************
    const InputMesh& InputScene::GetMesh(uint_t index) const
    {
        return *meshes_[index];
    }

    // **************************************************************************************
    uint_t InputScene::SearchMesh(const char* name) const
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
}
