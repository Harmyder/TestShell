#pragma once

namespace Harmyder
{
    class PointCloudRigid
    {
    public:
        PointCloudRigid(const XMFLOAT3 *points, uint32 pointsCount) :
            points_(points),
            pointsCount_(pointsCount)
        {}

        const XMFLOAT3* Points()       const { return points_; }
        const uint32    PointsCount()  const { return pointsCount_; }

    private:
        const XMFLOAT3 *points_;
        const uint32 pointsCount_;
    };

    class MeshRigid : private PointCloudRigid
    {
    public:
        MeshRigid(const XMFLOAT3 *vertices, uint32 verticesCount, Triangle *triangles, uint32 trianglesCount) :
            PointCloudRigid(vertices, verticesCount),
            triangles_(triangles),
            trianglesCount_(trianglesCount)
        {}

        const XMFLOAT3* Vertices()       const { return Points(); }
        const uint32    VerticesCount()  const { return PointsCount(); }
        const Triangle* Triangles()      const { return triangles_; }
        const uint32    TrianglesCount() const { return trianglesCount_; }

    private:
        const Triangle *triangles_;
        const uint32 trianglesCount_;
    };
}
