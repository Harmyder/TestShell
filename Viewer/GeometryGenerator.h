// **************************************************************************************
// Implements a static class which builds common geometrical objects
// All triangles are facing outward
// **************************************************************************************
#pragma once

#include "Vertex.h"

namespace Viewer
{
    class GeometryGenerator
    {
    public:

        typedef std::vector<VertexNormalTex> VerticesTriangle;
        typedef std::vector<VertexColor> VerticesLine;

    public:
        static void CreateCube(VerticesTriangle &output);
        static void CreateSphere(VerticesTriangle &output, uint32 iterations);
        // Cylinder is aligned along y-axis
        static void CreateCylinder(VerticesTriangle &output, float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount);
        static void CreateCone(VerticesTriangle &output, float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount);

        // Returns line list topology
        static void CreateGridXY(VerticesLine &output, uint32 xHalfCount, uint32 yHalfCount, float cellSizeX, float cellSizeY);
    };
}
