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

        typedef std::pair<std::vector<VertexNormalTex>, std::vector<uint16>> PredefinedGeometryData;
        typedef std::vector<VertexColor> VerticesLine;

    public:
        static PredefinedGeometryData CreateCube(float halfSide);
        static PredefinedGeometryData CreateBox(float halfSideX, float halfSideY, float halfSideZ);
        static PredefinedGeometryData CreateSphere(uint32 iterations);
        // Cylinder is aligned along y-axis
        static PredefinedGeometryData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount);
        static PredefinedGeometryData CreateCone(float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount);

        // Returns line list topology
        static VerticesLine CreateGratingXY(uint32 xHalfCount, uint32 yHalfCount, float cellSizeX, float cellSizeY);
    };
}
