#pragma once

namespace Pile
{
    class Timer
    {
    public:
        inline Timer(void);

        inline uint64 GetCounts();

        // returns time in seconds
        inline double GetTime();

        inline uint64 GetFrequency();
        inline double GetInvFrequency();

    private:
        uint64 m_Frequency;
        double m_InvFrequency;
    };

    inline Timer::Timer(void)
    {
        LARGE_INTEGER frequency;
        QueryPerformanceFrequency(&frequency);
        m_Frequency = static_cast<uint64>(frequency.QuadPart);

        m_InvFrequency = 1.0 / static_cast<double>(m_Frequency);
    }

    inline uint64 Timer::GetCounts()
    {
        LARGE_INTEGER counts;
        QueryPerformanceCounter(&counts);

        return static_cast<uint64>(counts.QuadPart);
    }

    inline double Timer::GetTime()
    {
        return GetCounts() * m_InvFrequency;
    }

    inline uint64 Timer::GetFrequency()
    {
        return m_Frequency;
    }

    inline double Timer::GetInvFrequency()
    {
        return m_InvFrequency;
    }
}
