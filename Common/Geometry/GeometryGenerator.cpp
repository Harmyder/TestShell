#include "stdafx.h"

#include "GeometryGenerator.h"

#include "Math\Vector\Vector.h"
#include "Math\Vector\Functions.h"
#include <unordered_map>
#include "Print\DebugPrint.h"
#include <DirectXMath.h>
using namespace std;

namespace std {
    template <> struct hash<Common::Vector3>
    {
        size_t operator()(const Common::Vector3 & x) const
        {
            XMFLOAT3 f3 = x.Store();
            return (int)f3.x * 2714011 ^ (int)f3.y * 1855519 ^ (int)f3.z * 1300297;
        }
    };
}

namespace Common
{
    bool GeometryGenerator::Vertex::operator==(const Vertex& other) {
        return
            other.PositionIndex == PositionIndex &&
            other.NormalIndex == NormalIndex &&
            other.TexCoordIndex == TexCoordIndex;
    }

    GeometryGenerator::UniqueVerticesAndTriangles GeometryGenerator::ComputeVertices(const std::vector<uint16>& trianglesPositions, const std::vector<uint16>& trianglesTexCoords) {
        UniqueVerticesAndTriangles output;
        const uint_t trianglesCount = trianglesPositions.size() / 3;
        output.TrianglesVertices.resize(trianglesCount * 3);
        for (uint_t t = 0; t < trianglesCount; t++) {
            for (uint16 v = 0; v < 3; v++) {
                Vertex vv;
                vv.PositionIndex = vv.NormalIndex = trianglesPositions[t * 3 + v];
                vv.TexCoordIndex = trianglesTexCoords[t * 3 + v];

                std::vector<Vertex>::iterator it = std::find(output.UniqueVertices.begin(), output.UniqueVertices.end(), vv);
                if (it == output.UniqueVertices.end()) {
                    output.UniqueVertices.push_back(vv);
                    it = output.UniqueVertices.end() - 1;
                }
                const uint_t vvIndex = it - output.UniqueVertices.begin();
                output.TrianglesVertices[t * 3 + v] = static_cast<uint16>(vvIndex);
            }
        }
        return output;
    }

    bool operator==(Vector3 v1, Vector3 v2) {
        XMFLOAT3 f1 = v1.Store();
        XMFLOAT3 f2 = v2.Store();
        return f1.x == f2.x && f1.y == f2.y && f1.z == f2.z;
    }

    vector<XMFLOAT3> ComputeNormals(const vector<XMFLOAT3>& positions, const vector<uint16>& trianglesPositions) {
        vector<Vector3> normals(positions.size(), Vector3(kZero));
        const uint32 trianglesCount = (uint32)(trianglesPositions.size() / 3);
        for (uint32 triangleIndex = 0; triangleIndex < trianglesCount; ++triangleIndex) {
            const uint32 baseVertexIndex = triangleIndex * 3;
            Vector3 a_ = XMLoadFloat3(&positions[trianglesPositions[baseVertexIndex]]);
            Vector3 b_ = XMLoadFloat3(&positions[trianglesPositions[baseVertexIndex + 1]]);
            Vector3 c_ = XMLoadFloat3(&positions[trianglesPositions[baseVertexIndex + 2]]);

            Vector3 ab = b_ - a_;
            Vector3 ac = c_ - a_;

            Vector3 normal = Cross(ab, ac);
            normals[trianglesPositions[baseVertexIndex]] += normal;
            normals[trianglesPositions[baseVertexIndex + 1]] += normal;
            normals[trianglesPositions[baseVertexIndex + 2]] += normal;
        }

        vector<XMFLOAT3> storedNormals; storedNormals.reserve(positions.size());
        for (auto& n : normals) storedNormals.push_back(Normalize(n).Store());
        return storedNormals;
    }

    // Fold vertices into distinct vertices and indices
    pair<vector<XMFLOAT3>, vector<uint16>> FoldVertices(const vector<Vector3>& vertices) {
        unordered_map<Vector3, int> vertexToIndex;
        for (const auto& v : vertices) {
            if (vertexToIndex.find(v) == vertexToIndex.end()) {
                vertexToIndex[v] = (int)vertexToIndex.size();
            }
        }

        vector<XMFLOAT3> positions(vertexToIndex.size());
        for (const auto& p : vertexToIndex) positions[p.second] = p.first.Store();

        const uint32 verticesCount = (uint32)vertices.size();
        const uint32 trianglesCount = verticesCount / 3;
        vector<uint16> trianglesPositions(verticesCount);
        for (uint32 triangleIndex = 0; triangleIndex < trianglesCount; ++triangleIndex) {
            const uint32 baseVertexIndex = triangleIndex * 3;
            trianglesPositions.push_back((uint16)vertexToIndex[vertices[baseVertexIndex]]);
            trianglesPositions.push_back((uint16)vertexToIndex[vertices[baseVertexIndex + 1]]);
            trianglesPositions.push_back((uint16)vertexToIndex[vertices[baseVertexIndex + 2]]);
        }

        return make_pair(positions, trianglesPositions);
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateBox(float halfSideX, float halfSideY, float halfSideZ) {
        Geometry output;

        output.Positions = {
            {-halfSideX, -halfSideY, -halfSideZ},
            {-halfSideX,  halfSideY, -halfSideZ},
            { halfSideX,  halfSideY, -halfSideZ},
            { halfSideX, -halfSideY, -halfSideZ},
            {-halfSideX, -halfSideY,  halfSideZ},
            {-halfSideX,  halfSideY,  halfSideZ},
            { halfSideX,  halfSideY,  halfSideZ},
            { halfSideX, -halfSideY,  halfSideZ}
        };

        output.TrianglesPositions = {
            // front face
            0, 1, 2,
            0, 2, 3,

            // back face
            4, 6, 5,
            4, 7, 6,

            // left face
            4, 5, 1,
            4, 1, 0,

            // right face
            3, 2, 6,
            3, 6, 7,

            // top face
            1, 5, 6,
            1, 6, 2,

            // bottom face
            4, 0, 3,
            4, 3, 7
        };

        output.Normals = ComputeNormals(output.Positions, output.TrianglesPositions);

        output.TexCoords = { {0.f, 0.f} };
        output.TrianglesTexCoords = vector<uint16>(output.TrianglesPositions.size(), 0);

        return output;
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateCube(float halfSide) {
        return CreateBox(halfSide, halfSide, halfSide);
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateSphere(uint32 iterations)
    {
        Geometry output;

        constexpr float radius = 1.f;

        // Build icosahedron, then on every iteration split every in four and project new point onto sphere surface
        constexpr uint32 facetsCount = 20;

        // Create 12 vertices of a icosahedron
        const float sqrt5 = sqrt(5.f);
        const float phi = (1.f + sqrt5) * .5f; // "golden ratio"
        // ratio of edge length to radius
        const float ratio = sqrt(10.f + (2.f * sqrt5)) / (4.f * phi);
        const float a = (radius / ratio) * .5f;
        const float b = (radius / ratio) / (2.f * phi);

        // http://paulbourke.net/geometry/platonic/sphere.cpp
        Vector3 P0(0, b, -a);
        Vector3 P1(b, a, 0);
        Vector3 P2(-b, a, 0);
        Vector3 P3(0, b, a);
        Vector3 P4(0, -b, a);
        Vector3 P5(-a, 0, b);
        Vector3 P6(0, -b, -a);
        Vector3 P7(a, 0, -b);
        Vector3 P8(a, 0, b);
        Vector3 P9(-a, 0, -b);
        Vector3 P10(b, -a, 0);
        Vector3 P11(-b, -a, 0);

        // Every iteration increases triangles number by factor 4 and we start with 4
        const uint32 trianglesCount = facetsCount * static_cast<uint32>(std::pow(4.f, static_cast<int>(iterations)));
        const uint32 verticesCount = trianglesCount * 3;

        // As we don't use index buffer we duplicate vertices much
        vector<Vector3> internalVertices(verticesCount, Vector3(kInfinity));
        vector<Vector3> internalNewVertices(verticesCount, Vector3(kInfinity));

        // Init vertices before the first iteration
        const Vector3 verticesC[facetsCount * 3] =
        {
            P0, P2, P1,
            P3, P1, P2,
            P3, P5, P4,
            P3, P4, P8,
            P0, P7, P6,
            P0, P6, P9,
            P4, P11, P10,
            P6, P10, P11,
            P2, P9, P5,
            P11, P5, P9,
            P1, P8, P7,
            P10, P7, P8,
            P3, P2, P5,
            P3, P8, P1,
            P0, P9, P2,
            P0, P1, P7,
            P6, P11, P9,
            P6, P7, P10,
            P4, P5, P11,
            P4, P10, P8,
        };

        internalVertices.assign(verticesC, verticesC + facetsCount * 3);

        // Refine triangles
        for (uint32 i = 0; i < iterations; ++i)
        {
            const uint32 stepTrianglesCount = facetsCount * static_cast<uint32>(std::pow(4.f, static_cast<int>(i)));
            for (uint32 triangleIndex = 0; triangleIndex < stepTrianglesCount; ++triangleIndex)
            {
                const uint32 vertexIndex = triangleIndex * 3;
                const auto vertex0 = internalVertices[vertexIndex];
                const auto vertex1 = internalVertices[vertexIndex + 1];
                const auto vertex2 = internalVertices[vertexIndex + 2];

                auto vertex01 = (vertex0 + vertex1) / 2.f;
                auto vertex12 = (vertex1 + vertex2) / 2.f;
                auto vertex20 = (vertex2 + vertex0) / 2.f;

                // Form 4 new triangles
                const uint32 baseNewTriangleIndex = triangleIndex * 4;
                uint32 currentVertexIndex = baseNewTriangleIndex * 3;

                // Project every new vertex onto sphere surface
                vertex01 = Normalize(vertex01) * radius;
                vertex12 = Normalize(vertex12) * radius;
                vertex20 = Normalize(vertex20) * radius;

                // Save
                internalNewVertices[currentVertexIndex++] = vertex0;
                internalNewVertices[currentVertexIndex++] = vertex01;
                internalNewVertices[currentVertexIndex++] = vertex20;

                internalNewVertices[currentVertexIndex++] = vertex01;
                internalNewVertices[currentVertexIndex++] = vertex1;
                internalNewVertices[currentVertexIndex++] = vertex12;

                internalNewVertices[currentVertexIndex++] = vertex12;
                internalNewVertices[currentVertexIndex++] = vertex2;
                internalNewVertices[currentVertexIndex++] = vertex20;

                internalNewVertices[currentVertexIndex++] = vertex01;
                internalNewVertices[currentVertexIndex++] = vertex12;
                internalNewVertices[currentVertexIndex++] = vertex20;
            }
            internalVertices.assign(internalNewVertices.begin(), internalNewVertices.begin() + stepTrianglesCount * 4 * 3);
        }

        tie(output.Positions, output.TrianglesPositions) = FoldVertices(internalVertices);

        output.Normals = ComputeNormals(output.Positions, output.TrianglesPositions);

        output.TexCoords = { { 0.f, 0.f } };
        output.TrianglesTexCoords = vector<uint16>(output.TrianglesPositions.size(), 0);

        return output;
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateCylinder(float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount)
    {
        Geometry output;

        const float minimumAllowedRadius = 0.01f;
        assert(slicesCount > 2 && stacksCount > 0 && bottomRadius >= minimumAllowedRadius);

        float stackHeight = height / stacksCount;

        // Amount to increment radius as we move up each stack level from bottom to top.
        float radiusStep = (topRadius - bottomRadius) / stacksCount;

        uint32 ringsCount = stacksCount + 1;

        const uint32 baseVerticesCount = ringsCount * slicesCount;
        vector<Vector3> baseVertices(baseVerticesCount, Vector3(kInfinity));

        // Compute all cylinder vertices starting at the bottom and moving up.
        uint32 vertexIndex = 0;
        for (uint32 i = 0; i < ringsCount; i++)
        {
            float y = -0.5f * height + i * stackHeight;
            float r = bottomRadius + i * radiusStep;

            if (r > minimumAllowedRadius)
            {
                // vertices of ring
                float dTheta = 2.0f * XM_PI / slicesCount;
                for (uint32 j = 0; j < slicesCount; ++j)
                {
                    float c = cosf(j * dTheta);
                    float s = sinf(j * dTheta);

                    baseVertices[vertexIndex++] = Vector3(r * c, y, r * s);
                }
            }
            else
            {
                for (uint32 j = 0; j < slicesCount; ++j)
                {
                    baseVertices[vertexIndex++] = Vector3(0.f, y, 0.f);
                }
            }
        }
        assert(vertexIndex == baseVerticesCount);

        const uint32 verticesCount = slicesCount * 3 * 2 + slicesCount * 3 * 2 * stacksCount;
        vector<Vector3> internalVertices;
        internalVertices.resize(verticesCount, Vector3(kInfinity));

        // Handle lateral surface first
        vertexIndex = 0;
        for (uint32 i = 0; i < stacksCount; i++)
        {
            for (uint32 j = 0; j < slicesCount; j++)
            {
                // B--D
                // |\ |
                // | \|
                // A--C
                const auto posA = baseVertices[i * slicesCount + j];
                const auto posC = baseVertices[i * slicesCount + (j + 1) % slicesCount];
                const auto posB = baseVertices[(i + 1) * slicesCount + j];
                const auto posD = baseVertices[(i + 1) * slicesCount + (j + 1) % slicesCount];

                internalVertices[vertexIndex++] = posA;
                internalVertices[vertexIndex++] = posB;
                internalVertices[vertexIndex++] = posC;

                internalVertices[vertexIndex++] = posC;
                internalVertices[vertexIndex++] = posB;
                internalVertices[vertexIndex++] = posD;
            }
        }

        // Handle bottom surface
        Vector3 bottomCenter(0.f, -0.5f * height, 0.f);
        for (uint32 j = 0; j < slicesCount; j++)
        {
            //    C(enter)
            //   / \
            //  /   \
            // A-----B
            const auto posA = baseVertices[j];
            const auto posB = baseVertices[(j + 1) % slicesCount];

            internalVertices[vertexIndex++] = posA;
            internalVertices[vertexIndex++] = posB;
            internalVertices[vertexIndex++] = bottomCenter;
        }

        // Handle top surface
        if (topRadius > minimumAllowedRadius)
        {
            Vector3 topCenter(0.f, 0.5f * height, 0.f);
            for (uint32 j = 0; j < slicesCount; j++)
            {
                //    C(enter)
                //   / \
                //  /   \
                // B-----A
                const auto posA = baseVertices[stacksCount * slicesCount + j];
                const auto posB = baseVertices[stacksCount * slicesCount + (j + 1) % slicesCount];

                internalVertices[vertexIndex++] = posB;
                internalVertices[vertexIndex++] = posA;
                internalVertices[vertexIndex++] = topCenter;
            }
        }

        tie(output.Positions, output.TrianglesPositions) = FoldVertices(internalVertices);

        output.Normals = ComputeNormals(output.Positions, output.TrianglesPositions);

        output.TexCoords = { { 0.f, 0.f } };
        output.TrianglesTexCoords = vector<uint16>(output.TrianglesPositions.size(), 0);

        return output;
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateCone(float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount)
    {
        return CreateCylinder(bottomRadius, 0.0f, height, slicesCount, stacksCount);
    }

    GeometryGenerator::Geometry GeometryGenerator::CreateGridXY(uint16 xCount, uint16 yCount, float width, float height) {
        Geometry output;
        output.Positions.resize(xCount * yCount);
        const Vector3 base(-width / 2.f, height / 2.f, 0.f);
        const float stepX = width / xCount;
        const float stepY = height / yCount;
        for (uint32 i = 0; i < xCount; ++i) {
            for (uint32 j = 0; j < yCount; ++j) {
                output.Positions[j * yCount + i] = Vector3(base + Vector3(i * stepX, j * -stepY, 0.f)).Store();
            }
        }
        // 0--h+0--2h+0
        // |\ |  \ |
        // | \|   \|
        // 1--h+1--2h+1
        const uint16 trianglesCount = 2 * (xCount - 1) * (yCount - 1);
        output.TrianglesPositions.resize(trianglesCount * 3);
        uint16 cur = 0;
        for (uint16 i = 0; i < xCount - 1; ++i) {
            for (uint16 j = 0; j < yCount - 1; ++j) {
                output.TrianglesPositions[cur++] = j + i * yCount;
                output.TrianglesPositions[cur++] = j + (i + 1) * yCount;
                output.TrianglesPositions[cur++] = j + (i + 1) * yCount + 1;
                output.TrianglesPositions[cur++] = j + i * yCount;
                output.TrianglesPositions[cur++] = j + (i + 1) * yCount + 1;
                output.TrianglesPositions[cur++] = (j + 1) + i * yCount;
            }
        }
        output.Normals.resize(output.Positions.size(), { 0.f, 0.f, 1.f });
        output.TexCoords.resize(output.Positions.size());
        for (uint16 i = 0; i < xCount; ++i) {
            for (uint16 j = 0; j < yCount; ++j) {
                output.TexCoords[i * xCount + j].x = float(i) / xCount;
                output.TexCoords[i * xCount + j].y = float(j) / yCount;
            }
        }
        output.TrianglesTexCoords = output.TrianglesPositions;
        return output;
    }
}
