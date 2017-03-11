#pragma once

namespace Common
{

    class NonCopyable
    {
    private:
        NonCopyable(const NonCopyable&) = delete;
        NonCopyable& operator = (const NonCopyable&) = delete;
    protected:
        NonCopyable() {}
    };

}
