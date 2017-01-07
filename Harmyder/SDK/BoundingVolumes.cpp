#include "stdafx.h"
#include "SDK\BoundingVolumes.h"

#include "SDK\Utility.h"
#include "Pile\Print\DumpDirectX.h"
#include "Pile\Print\DebugPrint.h"

using namespace std;
using namespace DirectX;

namespace Harmyder
{
    Sphere Compute(const XMFLOAT3 *vertices, uint32 verticesCount) {
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
        const XMFLOAT3 maxV = *(XMFLOAT3*)v.m[maxDiagI];
        
        uint32 minI, maxI;
        tie(minI, maxI) = ExtremePointsAlongDirection(maxV, vertices, verticesCount);

        XMVECTOR min = XMLoadFloat3(&vertices[minI]);
        XMVECTOR max = XMLoadFloat3(&vertices[maxI]);

        Sphere s;
        XMVECTOR radius = XMVectorDivide(XMVectorSubtract(max, min), XMVectorReplicate(2.f));
        s.radius = XMVectorGetX(XMVector3Length(radius));
        XMStoreFloat3(&s.center, XMVectorAdd(min, radius));

        EnlargeSphereByPoints(s, vertices, verticesCount);
        return s;
    }
}