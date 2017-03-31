#pragma once
#include "InputLevel/InputMesh.h"

#include "Common/Geometry/GeometryGenerator.h"

namespace Common {
    class Matrix4;
}

namespace Pipeline {
    namespace Helpers {
        void CreateBox(float x, float y, float z, const Common::Matrix4& transform, Pipeline::InputMesh& output);
        void CreateBoxInstanced(float x, float y, float z, const std::vector<Common::Matrix4>& transforms, InputMesh& output);

        void CreateGridXY(uint16 xCount, uint16 yCount, float width, float height, InputMesh& output);
    }
}

