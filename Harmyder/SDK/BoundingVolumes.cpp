#include "stdafx.h"
#include "SDK\BoundingVolumes.h"

#include "SDK\Utility.h"
#include "Common\Print\DumpDirectX.h"
#include "Common\Print\DebugPrint.h"

using namespace std;
using namespace DirectX;
using namespace Common;

namespace Harmyder
{
    Sphere Compute(const Vector3 *vertices, uint32 verticesCount) {
        auto covariance = CovarianceMatrix(vertices, verticesCount);

        XMFLOAT3X3 v;
        Jacobi(covariance, v);

        int maxDiagI = 0;
        float maxE = abs(covariance(0,0));
        if (abs(covariance(1,1)) > maxE) {
            maxDiagI = 1;
            maxE = abs(covariance(1, 1));
        }
        if (covariance(2, 2) > maxE) {
            maxDiagI = 2;
            maxE = abs(covariance(2, 2));
        }
        const Vector3 maxV = XMLoadFloat3((XMFLOAT3*)v.m[maxDiagI]);
        
        uint32 minI, maxI;
        tie(minI, maxI) = ExtremePointsAlongDirection(maxV, vertices, verticesCount);

        Vector3 min = vertices[minI];
        XMVECTOR max = vertices[maxI];

        Vector3 radius = (max - min) / 2.f;
        Sphere s{ min + radius, Length(radius) };

        EnlargeSphereByPoints(s, vertices, verticesCount);
        return s;
    }
}