#include "stdafx.h"

#include "GeometryGenerator.h"

using namespace DirectX;
using namespace std;

namespace Viewer
{
    void GeometryGenerator::CreateCube(VerticesTriangle &output)
    {
        const float side = 0.5f;
        XMFLOAT3 P0(-side, -side, -side);
        XMFLOAT3 P1(-side, side, -side);
        XMFLOAT3 P2(side, side, -side);
        XMFLOAT3 P3(side, -side, -side);
        XMFLOAT3 P4(-side, -side, side);
        XMFLOAT3 P5(-side, side, side);
        XMFLOAT3 P6(side, side, side);
        XMFLOAT3 P7(side, -side, side);

        XMFLOAT3 trianglesVertices[] =
        {
            // front face
            P0, P1, P2,
            P0, P2, P3,

            // back face
            P4, P6, P5,
            P4, P7, P6,

            // left face
            P4, P5, P1,
            P4, P1, P0,

            // right face
            P3, P2, P6,
            P3, P6, P7,

            // top face
            P1, P5, P6,
            P1, P6, P2,

            // bottom face
            P4, P0, P3,
            P4, P3, P7
        };

        XMFLOAT3 normals[6] =
        {
            // front face
            XMFLOAT3(0.f, 0.f, -1.f),
            // back face
            XMFLOAT3(0.f, 0.f, 1.f),
            // left face
            XMFLOAT3(-1.f, 0.f, 0.f),
            // right face
            XMFLOAT3(1.f, 0.f, 0.f),
            // top face
            XMFLOAT3(0.f, 1.f, 0.f),
            // bottom face
            XMFLOAT3(0.f, -1.f, 0.f),
        };

        XMFLOAT2 uvs[6] =
        {
            XMFLOAT2(0.f, 1.f),
            XMFLOAT2(1.f, 1.f),
            XMFLOAT2(0.f, 0.f),
            XMFLOAT2(0.f, 0.f),
            XMFLOAT2(1.f, 1.f),
            XMFLOAT2(1.f, 0.f),
        };

        output.reserve(36);
        for (uint32 i = 0; i < 36; ++i)
        {
            output.push_back(VertexNormalTex(trianglesVertices[i], normals[i / 6], uvs[i % 6]));
        }
    }

    void GeometryGenerator::CreateSphere(VerticesTriangle &output, uint32 iterations)
    {
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
        XMFLOAT3 P0(0, b, -a);
        XMFLOAT3 P1(b, a, 0);
        XMFLOAT3 P2(-b, a, 0);
        XMFLOAT3 P3(0, b, a);
        XMFLOAT3 P4(0, -b, a);
        XMFLOAT3 P5(-a, 0, b);
        XMFLOAT3 P6(0, -b, -a);
        XMFLOAT3 P7(a, 0, -b);
        XMFLOAT3 P8(a, 0, b);
        XMFLOAT3 P9(-a, 0, -b);
        XMFLOAT3 P10(b, -a, 0);
        XMFLOAT3 P11(-b, -a, 0);

        // Every iteration increases triangles number by factor 4 and we start with 4
        const uint32 trianglesCount = facetsCount * static_cast<uint32>(std::pow(4.f, static_cast<int>(iterations)));
        const uint32 verticesCount = trianglesCount * 3;

        // As we don't use index buffer we duplicate vertices much
        std::vector<XMFLOAT3> internalVertices(verticesCount);
        std::vector<XMFLOAT3> internalNewVertices(verticesCount);

        // Init vertices before the first iteration
        const XMFLOAT3 verticesC[facetsCount * 3] =
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
                const XMVECTOR vertex0 = XMLoadFloat3(&internalVertices[vertexIndex]);
                const XMVECTOR vertex1 = XMLoadFloat3(&internalVertices[vertexIndex + 1]);
                const XMVECTOR vertex2 = XMLoadFloat3(&internalVertices[vertexIndex + 2]);

                XMVECTOR vertex01 = (vertex0 + vertex1) / 2.f;
                XMVECTOR vertex12 = (vertex1 + vertex2) / 2.f;
                XMVECTOR vertex20 = (vertex2 + vertex0) / 2.f;

                // Form 4 new triangles
                const uint32 baseNewTriangleIndex = triangleIndex * 4;
                uint32 currentVertexIndex = baseNewTriangleIndex * 3;

                // Project every new vertex onto sphere surface
                vertex01 = XMVector3Normalize(vertex01) * radius;
                vertex12 = XMVector3Normalize(vertex12) * radius;
                vertex20 = XMVector3Normalize(vertex20) * radius;

                // Save
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex0);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex01);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex20);

                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex01);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex1);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex12);

                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex12);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex2);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex20);

                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex01);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex12);
                XMStoreFloat3(&internalNewVertices[currentVertexIndex++], vertex20);
            }
            internalVertices.assign(internalNewVertices.begin(), internalNewVertices.begin() + stepTrianglesCount * 4 * 3);
        }

        // Fill array of the vertices for the effect
        output.resize(verticesCount);
        for (uint32 i = 0; i < verticesCount; ++i)
        {
            output[i].Position = internalVertices[i];
        }

        // Calculate normals
        for (uint32 triangleIndex = 0; triangleIndex < trianglesCount; ++triangleIndex)
        {
            const uint32 baseVertexIndex = triangleIndex * 3;
            XMVECTOR a_ = XMLoadFloat3(&internalVertices[baseVertexIndex]);
            XMVECTOR b_ = XMLoadFloat3(&internalVertices[baseVertexIndex + 1]);
            XMVECTOR c_ = XMLoadFloat3(&internalVertices[baseVertexIndex + 2]);

            XMVECTOR ab = b_ - a_;
            XMVECTOR ac = c_ - a_;

            XMVECTOR normalXM = XMVector3Cross(ab, ac);

            XMStoreFloat3(&output[baseVertexIndex].Normal, normalXM);
            XMStoreFloat3(&output[baseVertexIndex + 1].Normal, normalXM);
            XMStoreFloat3(&output[baseVertexIndex + 2].Normal, normalXM);
        }

        for (auto& v : output) {
            XMStoreFloat3(&v.Normal, XMVector3Normalize(XMLoadFloat3(&v.Normal)));
        }
    }

    // Create cylinder along Y-axis
    void GeometryGenerator::CreateCylinder(VerticesTriangle &output, float bottomRadius, float topRadius, float height, uint32 slicesCount, uint32 stacksCount)
    {
        const float minimumAllowedRadius = 0.01f;
        assert(slicesCount > 2 && stacksCount > 0 && bottomRadius >= minimumAllowedRadius);

        float stackHeight = height / stacksCount;

        // Amount to increment radius as we move up each stack level from bottom to top.
        float radiusStep = (topRadius - bottomRadius) / stacksCount;

        uint32 ringsCount = stacksCount + 1;

        const uint32 baseVerticesCount = ringsCount * slicesCount;
        vector<XMFLOAT3> baseVertices(baseVerticesCount);

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

                    baseVertices[vertexIndex++] = XMFLOAT3(r * c, y, r * s);
                }
            }
            else
            {
                for (uint32 j = 0; j < slicesCount; ++j)
                {
                    baseVertices[vertexIndex++] = XMFLOAT3(0.f, y, 0.f);
                }
            }
        }
        assert(vertexIndex == baseVerticesCount);

        const uint32 verticesCount = slicesCount * 3 * 2 + slicesCount * 3 * 2 * stacksCount;
        output.resize(verticesCount);

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
                XMVECTOR posA = XMLoadFloat3(&baseVertices[i * slicesCount + j]);
                XMVECTOR posC = XMLoadFloat3(&baseVertices[i * slicesCount + (j + 1) % slicesCount]);
                XMVECTOR posB = XMLoadFloat3(&baseVertices[(i + 1) * slicesCount + j]);
                XMVECTOR posD = XMLoadFloat3(&baseVertices[(i + 1) * slicesCount + (j + 1) % slicesCount]);

                // Both triangles have the same normal
                XMVECTOR sideAB = posB - posA;
                XMVECTOR sideAC = posC - posA;
                XMVECTOR normal = XMVector3Cross(sideAB, sideAC);

                for (uint32 n = 0; n < 6; n++)
                {
                    XMStoreFloat3(&output[vertexIndex + n].Normal, normal);
                }

                XMStoreFloat3(&output[vertexIndex++].Position, posA);
                XMStoreFloat3(&output[vertexIndex++].Position, posB);
                XMStoreFloat3(&output[vertexIndex++].Position, posC);

                XMStoreFloat3(&output[vertexIndex++].Position, posC);
                XMStoreFloat3(&output[vertexIndex++].Position, posB);
                XMStoreFloat3(&output[vertexIndex++].Position, posD);
            }
        }

        // Handle bottom surface
        XMFLOAT3 bottomCenter(0.f, -0.5f * height, 0.f);
        XMFLOAT3 bottomNormal(0.f, -1.f, 0.f);
        for (uint32 n = 0; n < slicesCount * 3; n++)
        {
            output[vertexIndex + n].Normal = bottomNormal;
        }
        for (uint32 j = 0; j < slicesCount; j++)
        {
            //    C(enter)
            //   / \
            //  /   \
            // A-----B
            XMFLOAT3 posA = baseVertices[j];
            XMFLOAT3 posB = baseVertices[(j + 1) % slicesCount];

            output[vertexIndex++].Position = posA;
            output[vertexIndex++].Position = posB;
            output[vertexIndex++].Position = bottomCenter;
        }

        // Handle top surface
        if (topRadius > minimumAllowedRadius)
        {
            XMFLOAT3 topCenter(0.f, 0.5f * height, 0.f);
            XMFLOAT3 topNormal(0.f, 1.f, 0.f);
            for (uint32 n = 0; n < slicesCount * 3; n++)
            {
                output[vertexIndex + n].Normal = topNormal;
            }
            for (uint32 j = 0; j < slicesCount; j++)
            {
                //    C(enter)
                //   / \
                //  /   \
                // B-----A
                XMFLOAT3 posA = baseVertices[stacksCount * slicesCount + j];
                XMFLOAT3 posB = baseVertices[stacksCount * slicesCount + (j + 1) % slicesCount];

                output[vertexIndex++].Position = posB;
                output[vertexIndex++].Position = posA;
                output[vertexIndex++].Position = topCenter;
            }
        }
    }

    void GeometryGenerator::CreateCone(VerticesTriangle &vertices, float bottomRadius, float height, uint32 slicesCount, uint32 stacksCount)
    {
        CreateCylinder(vertices, bottomRadius, 0.0f, height, slicesCount, stacksCount);
    }

    void GeometryGenerator::CreateGridXY(VerticesLine &output, uint32 xHalfCount, uint32 yHalfCount, float cellSizeX, float cellSizeY) {
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
    }
}
