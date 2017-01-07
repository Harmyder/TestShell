#include "stdafx.h"
#include "SDK\Utility.h"

using namespace DirectX;
using namespace std;

namespace Harmyder
{
    XMFLOAT3X3 CovarianceMatrix(const XMFLOAT3* vertices, uint32 verticesCount) {
        XMVECTOR invN = XMVectorReplicate(1.f / verticesCount);
        XMVECTOR mean = g_XMZero;
        for (uint32 i = 0; i < verticesCount; ++i) {
            mean = XMVectorAdd(mean, XMLoadFloat3(&vertices[i]));
        }
        mean = XMVectorMultiply(mean, invN);

        XMVECTOR diag = g_XMZero;
        XMVECTOR upper = g_XMZero;
        for (uint32 i = 0; i < verticesCount; ++i) {
            XMVECTOR rel = XMLoadFloat3(&vertices[i]) - mean;
            diag = XMVectorMultiplyAdd(rel, rel, diag);
            upper = XMVectorMultiplyAdd(XMVectorSwizzle(rel, 0, 0, 1, 3), XMVectorSwizzle(rel, 1, 2, 2, 3), upper);
        }

        diag = XMVectorMultiply(diag, invN);
        upper = XMVectorMultiply(upper, invN);

        constexpr uint32 any = 0;
        XMFLOAT3X3 res;
        XMStoreFloat3x3(&res, XMMATRIX(
            XMVectorPermute<0, 4, 5, any>(diag, upper),
            XMVectorPermute<4, 1, 6, any>(diag, upper),
            XMVectorPermute<5, 6, 2, any>(diag, upper),
            g_XMZero
        ));
        return res;
    }

    // Golub, Van Loan "Matrix Computations", 5.1.8
    pair<float, float> Givens(float a, float b) {
        float s;
        float c;
        if (abs(b) < 0.0001f) {
            s = 1.f;
            c = 0.f;
        }
        else {
            if (abs(b) > abs(a)) {
                float t = -a / b;
                s = 1 / sqrt(1 + t * t);
                c = s * t;
            }
            else {
                float t = -b / a;
                c = 1 / sqrt(1 + t * t);
                s = c * t;
            }
        }
        return make_pair(c, s);
    }

    // Golub, Van Loan "Matrix Computations", 8.4.2
    pair<float, float> SymSchur2(const XMFLOAT3X3 &a, int p, int q)
    {
        float c, s;
        if (abs(a.m[p][q]) > 0.0001f) {
            float r = (a.m[q][q] - a.m[p][p]) / (2.0f * a.m[p][q]);
            float t;
            if (r >= 0.f)
                t = 1.f / (r + sqrt(1.f + r*r));
            else
                t = -1.f / (-r + sqrt(1.f + r*r));
            c = 1.f / sqrt(1.f + t*t);
            s = t * c;
        }
        else {
            c = 1.0f;
            s = 0.0f;
        }
        return make_pair(c, s);
    }
    
    // Ericson "Real-Time Collision Detection", 4.3.3
    void Jacobi(XMFLOAT3X3& a, XMFLOAT3X3& v) {
        XMMATRIX vInternal = XMMatrixIdentity();

        constexpr uint32 iterationsCount = 50;
        float offPrev = numeric_limits<float>::max();
        uint32 n;
        for (n = 0; n < iterationsCount; ++n) {
            int p = 1;
            int q = 0;
            if (abs(a.m[2][0]) > abs(a.m[p][q])) { p = 2; }
            if (abs(a.m[2][1]) > abs(a.m[p][q])) { p = 2; q = 1; }

            float s, c;
            tie(c, s) = SymSchur2(a, p, q);

            XMFLOAT3X3 raw;
            memset(&raw.m, 0, sizeof(float) * 9);
            raw.m[p][p] = raw.m[q][q] = c;
            raw.m[3 - p - q][3 - p - q] = 1;
            raw.m[p][q] = p < q ? -s : s;
            raw.m[q][p] = -raw.m[p][q];

            auto J = XMLoadFloat3x3(&raw);
            auto agT = XMMatrixMultiplyTranspose(XMLoadFloat3x3(&a), J);
            XMStoreFloat3x3(&a, XMMatrixMultiply(agT, J));

            vInternal = XMMatrixMultiply(vInternal, J);

            const float off = a.m[0][1] * a.m[0][1] + a.m[0][2] * a.m[0][2] + a.m[1][2] * a.m[1][2];
            if (off > offPrev) break;
        }
        XMStoreFloat3x3(&v, vInternal);
    }

    std::pair<uint32, uint32> ExtremePointsAlongDirection(const XMFLOAT3& dir, const XMFLOAT3* vertices, uint32 verticesCount) {
        assert(verticesCount > 0);
        XMVECTOR dirInternal = XMLoadFloat3(&dir);
        XMVECTOR minProj = XMVectorReplicate(numeric_limits<float>::max());
        XMVECTOR maxProj = XMVectorReplicate(numeric_limits<float>::lowest());
        for (uint32 i = 0; i < verticesCount; ++i) {
            auto vertex = XMLoadFloat3(&vertices[i]);
            auto proj = ::XMVector3Dot(dirInternal, vertex);

            XMVECTOR selectMin = XMVectorLess(proj, XMVectorSwizzle(minProj, 0, 0, 0, 0));
            XMVECTOR selectMax = XMVectorLess(XMVectorSwizzle(maxProj, 0, 0, 0, 0), proj);
            minProj = ::XMVectorSelect(minProj, XMVectorSetW(proj, *(float*)&i), selectMin);
            maxProj = ::XMVectorSelect(maxProj, XMVectorSetW(proj, *(float*)&i), selectMax);
        }
        const float min = XMVectorGetW(minProj);
        const float max = XMVectorGetW(maxProj);
        return make_pair(*(uint32*)&min, *(uint32*)&max);
    }

    void EnlargeSphereByPoints(Sphere& s, const XMFLOAT3* points, const uint32 pointsCount) {
        XMVECTOR r = XMVectorReplicate(s.radius);
        float r2 = s.radius * s.radius;
        XMVECTOR c = XMLoadFloat3(&s.center);
        for (uint32 i = 0; i < pointsCount; ++i) {
            XMVECTOR d = XMVectorSubtract(XMLoadFloat3(&points[i]), c);
            const XMVECTOR dist2 = XMVector3Dot(d, d);
            if (r2 < XMVectorGetX(dist2)) {
                float dist = sqrt(XMVectorGetX(dist2));
                float newRadius = (s.radius + dist) * .5f;
                float k = (newRadius - s.radius) / dist;
                s.radius = newRadius;
                r2 = s.radius * s.radius;
                c = XMVectorMultiplyAdd(d, XMVectorReplicate(k), c);
                XMStoreFloat3(&s.center, c);
            }
            assert(s.radius >= XMVectorGetX(XMVector3Length(XMVectorSubtract(XMLoadFloat3(&points[i]), c))));
        }
    }
}