#pragma once
#include "InputLevel/InputMesh.h"

#include "Common/Geometry/GeometryGenerator.h"

namespace Common {
    class OrthogonalTransform;
}

namespace Pipeline {
    namespace Helpers {
        void CreateBox(float x, float y, float z, const Common::OrthogonalTransform& transform, Pipeline::InputMesh& output);
        void CreateBoxInstanced(float x, float y, float z, const std::vector<Common::OrthogonalTransform>& transforms, InputMesh& output);

        
        void CreateGridXyInternal(Common::GeometryGenerator::Geometry& grid, InputMesh& output);
        
        inline void CreateGridXy(uint16 xCount, uint16 yCount, float width, float height, Common::GeometryGenerator::QuadDivision division, InputMesh& output, std::function<float(int, int)> getZ = [](int,int) { return 0.f; }) {
            auto grid = Common::GeometryGenerator::CreateGridXy(xCount, yCount, width, height, division, getZ);
            CreateGridXyInternal(grid, output);
        }
    }
}

