#pragma once
#include "stdafx.h"
#include "CppUnitTest.h"

#include <sstream>
#include <iomanip> 

#include <DirectXMath.h>

using namespace DirectX;
using namespace std;

float EPSILON = 0.0001f;

bool CompareEpsilon(float x, float y) {
    return abs(x - y) <= EPSILON * max(1.f, max(abs(x), abs(y)));
}

bool operator==(const XMFLOAT3X3& p1, const XMFLOAT3X3& p2) {
    bool res = true;
    for (int i = 0; i < 3; ++i) {
        for (int j = 0; j < 3; ++j) {
            res &= CompareEpsilon(p1(i, j), p2(i, j));
        }
    }
    return res;
}

bool operator==(const XMFLOAT4X4& p1, const XMFLOAT4X4& p2) {
    bool res = true;
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            res &= CompareEpsilon(p1(i, j), p2(i, j));
        }
    }
    return res;
}

bool operator==(const XMFLOAT3& p1, const XMFLOAT3& p2) {
    bool res = true;
    res &= CompareEpsilon(p1.x, p2.x);
    res &= CompareEpsilon(p1.y, p2.y);
    res &= CompareEpsilon(p1.z, p2.z);
    return res;
}

bool operator==(const XMFLOAT4& p1, const XMFLOAT4 p2) {
    bool res = true;
    res &= CompareEpsilon(p1.x, p2.x);
    res &= CompareEpsilon(p1.y, p2.y);
    res &= CompareEpsilon(p1.z, p2.z);
    res &= CompareEpsilon(p1.w, p2.w);
    return res;
}

bool operator==(const Vector3& p1, const Vector3& p2) {
    return p1.Store() == p2.Store();
}

bool operator==(const Matrix3& p1, const Matrix3& p2) {
    return p1.Store() == p2.Store();
}


namespace Microsoft {
namespace VisualStudio {
namespace CppUnitTestFramework {

    template<> inline std::wstring ToString<XMFLOAT3>(const XMFLOAT3& t) {
        wstringstream ss;
        ss << setw(6) << setprecision(2) << endl;
        ss << "{" << t.x << ", " << t.y << ", " << t.z << "}";
        wstring res = ss.str();
        return res;
    }

    template<> inline std::wstring ToString<XMFLOAT4>(const XMFLOAT4& t) {
        wstringstream ss;
        ss << setw(6) << setprecision(2);
        ss << "{" << t.x << ", " << t.y << ", " << t.z << ", " << t.w << "}";
        wstring res = ss.str();
        return res;
    }

    template<> inline std::wstring ToString<XMFLOAT3X3>(const XMFLOAT3X3& t) {
        wstringstream ss;
        ss << setw(6) << setprecision(2) << endl;
        ss << t._11 << " " << t._12 << " " << t._13 << endl;
        ss << t._21 << " " << t._22 << " " << t._23 << endl;
        ss << t._31 << " " << t._32 << " " << t._33 << endl;
        wstring res = ss.str();
        return res;
    }

    template<> inline std::wstring ToString<XMFLOAT4X4>(const XMFLOAT4X4& t) {
        wstringstream ss;
        ss << setw(6) << setprecision(2) << endl;
        ss << t._11 << " " << t._12 << " " << t._13 << " " << t._14 << endl;
        ss << t._21 << " " << t._22 << " " << t._23 << " " << t._24 << endl;
        ss << t._31 << " " << t._32 << " " << t._33 << " " << t._34 << endl;
        ss << t._41 << " " << t._42 << " " << t._43 << " " << t._44 << endl;
        wstring res = ss.str();
        return res;
    }

    template<> inline std::wstring ToString<Vector3>(const Vector3& t) {
        return ToString(t.Store());
    }

    template<> inline std::wstring ToString<Matrix3>(const Matrix3& t) {
        return ToString(t.Store());
    }
}
}
}
