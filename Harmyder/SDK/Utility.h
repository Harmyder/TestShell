#pragma once

namespace Harmyder
{
    XMFLOAT3X3 CovarianceMatrix(const XMFLOAT3* vertices, uint32 verticesCount);

    std::pair<float, float> Givens(float a, float b);

    void Jacobi(XMFLOAT3X3& a, XMFLOAT3X3& v);

    std::pair<uint32, uint32> ExtremePointsAlongDirection(const XMFLOAT3& dir, const XMFLOAT3* vertices, uint32 verticesCount);

    void EnlargeSphereByPoints(Sphere& s, const XMFLOAT3* points, const uint32 pointsCount);
}