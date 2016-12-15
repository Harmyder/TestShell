#pragma once

namespace Pile
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
