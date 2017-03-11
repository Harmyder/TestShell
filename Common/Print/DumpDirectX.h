#pragma once

#include <DirectXMath.h>

namespace Common
{
    enum class DumpStyle {
        kVisualStudio,
        kMatlab,
    };

    std::string Dump(const DirectX::XMFLOAT3* points, uint32 pointsCount, DumpStyle style, bool isEndLine);

    std::string Dump(const DirectX::XMFLOAT3X3& matrix, DumpStyle style, bool isEndLine);

    std::string Dump(const DirectX::XMFLOAT4X4& matrix, DumpStyle style, bool isEndLine);
}
