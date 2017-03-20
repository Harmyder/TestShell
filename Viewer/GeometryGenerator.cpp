#include "stdafx.h"

#include "GeometryGenerator.h"
#include "Common/Geometry/GeometryGenerator.h"

using namespace DirectX;
using namespace std;

namespace Viewer
{
    static GeometryGenerator::PredefinedGeometryData GeometryToPredefinedGeometryData(const Common::GeometryGenerator::Geometry& geometry) {
        auto v = Common::GeometryGenerator::ComputeVertices(geometry.TrianglesPositions, geometry.TrianglesTexCoords);
        vector<VertexNormalTex> uniqueVertices(v.UniqueVertices.size());
        for (uint32 i = 0; i < v.UniqueVertices.size(); ++i) {
            uniqueVertices[i] = {
                geometry.Positions[v.UniqueVertices[i].PositionIndex],
                geometry.Normals[v.UniqueVertices[i].NormalIndex],
                geometry.TexCoords[v.UniqueVertices[i].TexCoordIndex] };
        }
        GeometryGenerator::PredefinedGeometryData output;
        output.first = move(uniqueVertices);
        output.second = v.TrianglesVertices;
        return output;
    }

    GeometryGenerator::PredefinedGeometryData GeometryGenerator::CreateBox(float halfSideX, float halfSideY, float halfSideZ) {
        auto geometry = Common::GeometryGenerator::CreateBox(halfSideX, halfSideY, halfSideZ);
        return GeometryToPredefinedGeometryData(geometry);
    }

    GeometryGenerator::PredefinedGeometryData GeometryGenerator::CreateCube(float halfSide) {
        return CreateBox(halfSide, halfSide, halfSide);
    }

    GeometryGenerator::PredefinedGeometryData GeometryGenerator::CreateSphere(uint32 iterations) {
        auto geometry = Common::GeometryGenerator::CreateSphere(iterations);
        return GeometryToPredefinedGeometryData(geometry);
    }

    // Create cylinder along Y-axis
    GeometryGenerator::PredefinedGeometryData GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount) {
        auto geometry = Common::GeometryGenerator::CreateCylinder(bottomRadius, topRadius, height, slicesCount, stacksCount);
        return GeometryToPredefinedGeometryData(geometry);
    }

    GeometryGenerator::PredefinedGeometryData GeometryGenerator::CreateCone(float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount) {
        return CreateCylinder(bottomRadius, 0.0f, height, slicesCount, stacksCount);
    }

    GeometryGenerator::VerticesLine GeometryGenerator::CreateGratingXY(uint32 xHalfCount, uint32 yHalfCount, float cellSizeX, float cellSizeY) {
        VerticesLine output;

        const float baseX = -1.f * (xHalfCount + 1) * cellSizeX;
        const float baseY = -1.f * (yHalfCount + 1) * cellSizeY;
        const float lengthX = -baseX * 2.f;
        const float lengthY = -baseY * 2.f;

        XMFLOAT4 color(.5f, .5f, .5f, .5f);
        const uint32 xCount = xHalfCount * 2 + 1;
        for (uint32 i = 1; i <= xCount; ++i) {
            if (i == xHalfCount + 1) continue;
            VertexColor p;
            p.Color = color;
            p.Position = XMFLOAT3(baseX + i * cellSizeX, baseY, 0.f);
            output.push_back(p);
            p.Position.y += lengthY;
            output.push_back(p);
        }

        const uint32 yCount = yHalfCount * 2 + 1;
        for (uint32 j = 1; j <= yCount; ++j) {
            if (j == yHalfCount + 1) continue;
            VertexColor p;
            p.Color = color;
            p.Position = XMFLOAT3(baseX, baseY + j * cellSizeY, 0.f);
            output.push_back(p);
            p.Position.x += lengthX;
            output.push_back(p);
        }
        return output;
    }
}
