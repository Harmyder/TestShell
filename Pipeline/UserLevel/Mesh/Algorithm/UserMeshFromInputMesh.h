// **************************************************************************************
// Desc: Fills Mesh with data from its InputMesh
// **************************************************************************************
#pragma once

#include "Pile\Attribute\NonCopyable.h"

namespace Pipeline
{
    class UserMesh;
    class InputMesh;

    class UserMeshFromInputMesh : Pile::NonCopyable
    {
    public:
        UserMeshFromInputMesh(UserMesh &mesh);

        void Restore();

    private:
        UserMesh &mesh_;
        const InputMesh& inputMesh_;
    };
}
