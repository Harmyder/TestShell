#pragma once

#include <DirectXMath.h>
#include <functional>

namespace Common
{
    class GeometryGenerator
    {
    public:
        struct Geometry {
            std::vector<DirectX::XMFLOAT3> Positions;
            std::vector<DirectX::XMFLOAT3> Normals;
            std::vector<uint16>   TrianglesPositions;
            std::vector<DirectX::XMFLOAT2> TexCoords;
            std::vector<uint16>   TrianglesTexCoords;
        };

        struct Vertex
        {
            uint16 PositionIndex;
            uint16 NormalIndex;
            uint16 TexCoordIndex;

            bool operator==(const Vertex& other);
        };

        struct UniqueVerticesAndTriangles {
            std::vector<Vertex> UniqueVertices;
            std::vector<uint16> TrianglesVertices;
        };

        static UniqueVerticesAndTriangles ComputeVertices(const std::vector<uint16>& trianglesPositions, const std::vector<uint16>& trianglesTexCoords);
        
    public:
        static Geometry CreateCube(float halfSide);
        static Geometry CreateBox(float halfSideX, float halfSideY, float halfSideZ);
        static Geometry CreateSphere(uint32 iterations);
        // Cylinder is aligned along y-axis
        static Geometry CreateCylinder(float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount);
        static Geometry CreateCone(float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount);
        // heights exepected to be row-major
        static Geometry CreateGridXY(uint16 xCount, uint16 yCount, float width, float height, const std::function<float(int)>& getHeight = [](int) { return 0.f; });
    };
}
