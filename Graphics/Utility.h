#pragma once
#include "stdafx.h"

namespace Graphics
{
namespace Utility
{
    std::string inline basename(std::string const& pathname) {
        return std::string(std::find_if(pathname.rbegin(), pathname.rend(), [](char ch) { return ch == '\\'; }).base(), pathname.end());
    }

    template <typename T> bool IsPowerOfTwo(T value) {
        return 0 == (value & (value - 1));
    }

    template <typename T> T AlignUpWithMask(T value, size_t mask) {
        return (T)(((size_t)value + mask) & ~mask);
    }

    template <typename T> T AlignUp(T value, size_t alignment) {
        assert(IsPowerOfTwo(alignment));
        return AlignUpWithMask(value, alignment - 1);
    }

    template <typename T> bool IsAligned(T value, size_t alignment) {
        assert(IsPowerOfTwo(alignment));
        return value & (alignment - 1) == 0;
    }

    uint32 inline CalcConstBufSize(uint_t unalignedSize) {
        return (uint32)AlignUp(unalignedSize, 256);
    }
}

#define THROW_IF_FAILED(hr) \
    if (FAILED(hr)) { \
        auto err = _com_error(hr); \
        Common::DebugPrintf("Error in %s@%d: %s\n", Utility::basename(__FILE__).c_str(), __LINE__, err.ErrorMessage()); \
        DebugBreak(); \
    }
}