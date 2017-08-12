#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common/Math/Numerical/GaussQuadrature.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(GaussQuadrature_Test)
    {
        template <int N, class F>
        void TestGaussQuadrature(const F& f, double a, double b, double sum, double delta = 0.) {
            double actual = GaussQuadrature<N, F>::Compute(f, a, b);
            Assert::AreEqual(sum, actual, delta);
        }

    public:

        TEST_METHOD(TestGaussQuadrature_ConstantNormalized) { TestGaussQuadrature<1>([](double) { return 1; }, -1., 1., 2.); }
        TEST_METHOD(TestGaussQuadrature_ConstantShifted) { TestGaussQuadrature<1>([](double) { return 1; }, 1., 3., 2.); }
        TEST_METHOD(TestGaussQuadrature_ConstantScaled) { TestGaussQuadrature<1>([](double) { return 1; }, -2., 2., 4.); }
        TEST_METHOD(TestGaussQuadrature_Constant) { TestGaussQuadrature<1>([](double) { return 1; }, 3., 7., 4.); }

        TEST_METHOD(TestGaussQuadrature_LinearNormalized) { TestGaussQuadrature<1>([](double x) { return x; }, -1., 1., 0.); }
        TEST_METHOD(TestGaussQuadrature_LinearShifted) { TestGaussQuadrature<1>([](double x) { return x; }, 1., 3., 4.); }
        TEST_METHOD(TestGaussQuadrature_LinearScaled) { TestGaussQuadrature<1>([](double x) { return x; }, -2., 2., 0.); }
        TEST_METHOD(TestGaussQuadrature_Linear) { TestGaussQuadrature<1>([](double x) { return x; }, 3., 7., 20.); }

        TEST_METHOD(TestGaussQuadrature_QuadraticNormalized) { TestGaussQuadrature<2>([](double x) { return x * x; }, -1., 1., 2./3.); }
        TEST_METHOD(TestGaussQuadrature_QuadraticShifted) { TestGaussQuadrature<2>([](double x) { return x * x; }, 1., 3., 26./3.); }
        TEST_METHOD(TestGaussQuadrature_QuadraticScaled) { TestGaussQuadrature<2>([](double x) { return x * x; }, -2., 2., 16./3.); }
        TEST_METHOD(TestGaussQuadrature_Quadratic) { TestGaussQuadrature<2>([](double x) { return x * x; }, 3., 7., 316./3.); }

        TEST_METHOD(TestGaussQuadrature_CubicNormalized) { TestGaussQuadrature<2>([](double x) { return x * x * x; }, -1., 1., 0.); }
        TEST_METHOD(TestGaussQuadrature_CubicShifted) { TestGaussQuadrature<2>([](double x) { return x * x * x; }, 1., 3., 20.); }
        TEST_METHOD(TestGaussQuadrature_CubicScaled) { TestGaussQuadrature<2>([](double x) { return x * x * x; }, -2., 2., 0.); }
        TEST_METHOD(TestGaussQuadrature_Cubic) { TestGaussQuadrature<2>([](double x) { return x * x * x; }, 3., 7., 580.); }

        const double kEpsilon = 1e-5;
        TEST_METHOD(TestGaussQuadrature_QuarticNormalized) { TestGaussQuadrature<3>([](double x) { return pow(x, 4.); }, -1., 1., .4, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_QuarticShifted) { TestGaussQuadrature<3>([](double x) { return pow(x, 4.); }, 1., 3., 48.4, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_QuarticScaled) { TestGaussQuadrature<3>([](double x) { return pow(x, 4.); }, -2., 2., 12.8, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_Quartic) { TestGaussQuadrature<3>([](double x) { return pow(x, 4.); }, 3., 7., 3312.8, kEpsilon); }

        TEST_METHOD(TestGaussQuadrature_QuinticNormalized) { TestGaussQuadrature<3>([](double x) { return pow(x, 5.) + 1.; }, -1., 1., 2, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_QuinticShifted) { TestGaussQuadrature<3>([](double x) { return pow(x, 5.) + 1.; }, 1., 3., 123.333333, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_QuinticScaled) { TestGaussQuadrature<3>([](double x) { return pow(x, 5.) + 1.; }, -2., 2., 4, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_Quintic) { TestGaussQuadrature<3>([](double x) { return pow(x, 5.) + 1.; }, 3., 7., 19490.666666, kEpsilon); }

        TEST_METHOD(TestGaussQuadrature_SexticNormalized) { TestGaussQuadrature<4>([](double x) { return pow(x, 6.) + 1.; }, -1., 1., 2.285714, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_SexticShifted) { TestGaussQuadrature<4>([](double x) { return pow(x, 6.) + 1.; }, 1., 3., 314.285714, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_SexticScaled) { TestGaussQuadrature<4>([](double x) { return pow(x, 6.) + 1.; }, -2., 2., 40.571428, kEpsilon); }
        TEST_METHOD(TestGaussQuadrature_Sextic) { TestGaussQuadrature<4>([](double x) { return pow(x, 6.) + 1.; }, 3., 7., 117340.571428, kEpsilon); }
    };
}
