#pragma once

namespace Common
{
    static inline DirectX::XMFLOAT4X4 Identity4x4()
    {
        static DirectX::XMFLOAT4X4 I(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f);

        return I;
    }

    constexpr DirectX::XMFLOAT3 kXmFloat3Zero(0.f, 0.f, 0.f);
    constexpr DirectX::XMFLOAT3 kXmFloat3One(1.f, 1.f, 1.f);
}