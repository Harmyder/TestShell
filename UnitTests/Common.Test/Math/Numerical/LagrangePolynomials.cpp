#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common/Math/Numerical/LagrangePolynomials.h"
#include "Common/Container/Iterator.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;
using namespace std;

namespace CommonTest
{
    TEST_CLASS(LagrangeInterpolation_Test)
    {
    public:

        TEST_METHOD(TestLagrangeInterpolation) {
            struct Point { double x, y; };
            struct AbscissaIterator : public Common::RandomAccessIterator<vector<Point>, const double, AbscissaIterator> {
                using Common::RandomAccessIterator<vector<Point>, const double, AbscissaIterator>::RandomAccessIterator;
                value_type& operator* () override { return container_[index_].x; }
                value_type* operator->() override { return &container_[index_].x; }
            };
            struct OrdinateIterator : public Common::RandomAccessIterator<vector<Point>, const double, OrdinateIterator> {
                using Common::RandomAccessIterator<vector<Point>, const double, OrdinateIterator>::RandomAccessIterator;
                value_type& operator* () override { return container_[index_].x; }
                value_type* operator->() override { return &container_[index_].x; }
            };
            vector<Point> points;
            const int degree = 5;
            const int ctrlPtsCount = degree + 1;
            const double a = 0.;
            const double b = 4.;
            const double step = (b - a) / degree;
            for (int i = 0; i < ctrlPtsCount; ++i) {
                const double x = a + step * i;
                const double y = pow(x, degree);
                points.push_back({ x, y });
            }

            LagrangeInterpolation<AbscissaIterator, OrdinateIterator> li(degree, AbscissaIterator(points), OrdinateIterator(points));
            for (const auto& p : points) {
                Assert::AreEqual(p.y, li(p.x));
            }
        }
    };
}