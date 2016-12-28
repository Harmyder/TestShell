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

        typedef std::vector<Vertex> Vertices;

    public:
        static void CreateCube(Vertices &output);
        static void CreateSphere(Vertices &output, uint32 iterations);
        // Cylinder is aligned along y-axis
        static void CreateCylinder(Vertices &output, float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount);
        static void CreateCone(Vertices &output, float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount);
    };
}
