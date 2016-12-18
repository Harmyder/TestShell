// **************************************************************************************
// Desc: Fills Mesh with data from its InputMesh
// **************************************************************************************
#pragma once

#include "Pile\Attribute\NonCopyable.h"

namespace Viewer {
    class Viewport;
}

namespace Pipeline
{
    class UserMesh;
    class InputMesh;

    class UserMeshFromInputMesh : Pile::NonCopyable
    {
    public:
        UserMeshFromInputMesh(UserMesh &mesh);

        void Restore(Viewer::Viewport& viewport);

    private:
        UserMesh &mesh_;
        const InputMesh& inputMesh_;
    };
}
