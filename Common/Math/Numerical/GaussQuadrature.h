#pragma once
#include <array>

namespace Common
{
    struct WeightAbscissa {
        double w;
        double a;
    };

    constexpr int N = 5;
    constexpr int QuadratureStart(int n) { return (1 + n) * n / 2; }
    using WAS = std::array<WeightAbscissa, QuadratureStart(N)>;
    constexpr WAS kWas = { {
        { 2.0000000000000000, 0.0000000000000000 },

        { 1.0000000000000000, -0.5773502691896257 },
        { 1.0000000000000000, 0.5773502691896257 },

        { 0.8888888888888888, 0.0000000000000000 },
        { 0.5555555555555556, -0.7745966692414834 },
        { 0.5555555555555556, 0.7745966692414834 },

        { 0.6521451548625461, -0.3399810435848563 },
        { 0.6521451548625461, 0.3399810435848563 },
        { 0.3478548451374538, -0.8611363115940526 },
        { 0.3478548451374538, 0.8611363115940526 },

        { 0.5688888888888889, 0.0000000000000000 },
        { 0.4786286704993665, -0.5384693101056831 },
        { 0.4786286704993665, 0.5384693101056831 },
        { 0.2369268850561891, -0.9061798459386640 },
        { 0.2369268850561891, 0.9061798459386640 }
    } };
    // Until C++17 std::array::begin is not a constexpr
    const std::array<std::pair<WAS::const_iterator, WAS::const_iterator>, N> kWasPerN = { {
        { kWas.cbegin() + QuadratureStart(0), kWas.cbegin() + QuadratureStart(1) },
        { kWas.cbegin() + QuadratureStart(1), kWas.cbegin() + QuadratureStart(2) },
        { kWas.cbegin() + QuadratureStart(2), kWas.cbegin() + QuadratureStart(3) },
        { kWas.cbegin() + QuadratureStart(3), kWas.cbegin() + QuadratureStart(4) },
        { kWas.cbegin() + QuadratureStart(4), kWas.cbegin() + QuadratureStart(5) },
    } };

    template <int N, class F>
    class GaussQuadrature
    {
    public:
        static double Compute(const F& f, double a, double b) {
            const double shift = (a + b) / 2.;
            const double scale = (b - a) / 2.;
            double sum = 0;
            for (auto wa = kWasPerN[N - 1].first; wa < kWasPerN[N - 1].second; ++wa) {
                sum += f(wa->a * scale + shift) * wa->w;
            }
            return sum * scale;
        }
    };
}
