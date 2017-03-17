#include "stdafx.h"
#include "CppUnitTest.h"

#include "Harmyder\SDK\Utility.h"

#include "ToString.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace HarmyderTest
{		
	TEST_CLASS(Utility)
	{
	public:
		
		TEST_METHOD(TestCovarianceMatrix_Simple) {
            Vector3 vertices[3] = { {1.f, 2.f, 0.f}, {2.f, 4.f, 0.f}, {3.f, 6.f, 0.f} };
            auto cov = Harmyder::CovarianceMatrix(vertices, 3);
            const XMFLOAT3X3 expected(
                0.6667f, 1.3333f, 0.f,
                1.3333f, 2.6667f, 0.f,
                0.f,     0.f,     0.f);
            Assert::AreEqual(expected, cov);
		}

        // MATLAB:
        // A = [1 2 0; 2 4 0; 3 6 0; 2 4 0; 2 6 1; 3 5 7; 1 2 4];
        // cov(A, 1)
        TEST_METHOD(TestCovarianceMatrix_Complex) {
            vector<Vector3> vertices = { 
                { 1.f, 2.f, 0.f },
                { 2.f, 4.f, 0.f },
                { 3.f, 6.f, 0.f },
                { 2.f, 4.f, 0.f },
                { 2.f, 6.f, 1.f },
                { 3.f, 5.f, 7.f },
                { 1.f, 2.f, 4.f } };
            auto cov = Harmyder::CovarianceMatrix(vertices.data(), (uint32)vertices.size());
            const XMFLOAT3X3 expected(
                0.5714f,  1.f,      0.4286f,
                1.f,      2.4082f, -0.1020f,
                0.4286f, -0.102f,   6.4898f);
            Assert::AreEqual(expected, cov);
        }

        TEST_METHOD(TestGivens_1) {
            float c, s;
            tie(c, s) = Harmyder::Givens(6, 5);
            float expectedC = .7682f;
            float expectedS = -.6402f;
            Assert::AreEqual(expectedC, c, EPSILON);
            Assert::AreEqual(expectedS, s, EPSILON);
        }

        TEST_METHOD(TestGivens_2) {
            float c, s;
            tie(c, s) = Harmyder::Givens(-2.4327f, 4);
            float expectedC = .5196f;
            float expectedS = .8544f;
            Assert::AreEqual(expectedC, c, EPSILON);
            Assert::AreEqual(expectedS, s, EPSILON);
        }

        TEST_METHOD(TestJacobi) {
            XMFLOAT3X3 a(2, 1, 1, 1, 2, 1, 1, 1, 2);
            XMFLOAT3X3 v;
            Harmyder::Jacobi(a, v);
            float actual[3] = { a._11, a._22, a._33 };
            float expected[3] = { 1, 1, 4 };
            for (int i = 0; i < 3; ++i) {
                float ee = expected[i];
                Assert::IsTrue(find_if(cbegin(actual), cend(actual),
                    [ee](float f) {
                    for (int i = 0; i < 3; ++i) {
                        if (CompareEpsilon(f, ee)) return true;
                    }
                    return false;
                }) < actual + 3);
            }
        }

        TEST_METHOD(TestEnlargeSphereByPoints_AlongAxis) {
            Harmyder::Sphere s{ { 0, 0, 0 }, 1 };
            Vector3 point(2, 0, 0);
            Harmyder::EnlargeSphereByPoints(s, &point, 1);

            Assert::AreEqual(float(s.radius), 1.5f);
            Assert::AreEqual(s.center, Vector3(0.5, 0, 0));
        }
    
        TEST_METHOD(TestEnlargeSphereByPoints_DiagonalXY) {
            Harmyder::Sphere s{ { 1.f, 1.f, 0.f }, sqrt(2.f) };
            Vector3 point(3.f, 3.f, 0.f);
            Harmyder::EnlargeSphereByPoints(s, &point, 1);

            const float c = 1.5f;
            const float r = sqrt(1.5f * 1.5f * 2.f);
            Assert::IsTrue(CompareEpsilon(s.radius, r));
            Assert::AreEqual(s.center, Vector3(c, c, 0.f));
        }

        TEST_METHOD(TestEnlargeSphereByPoints_DiagonalXYZ) {
            Harmyder::Sphere s{ { 1.f, 1.f, 1.f }, sqrt(3.f) };
            Vector3 point(3.f, 3.f, 3.f);
            Harmyder::EnlargeSphereByPoints(s, &point, 1);

            const float c = 1.5f;
            const float r = sqrt(1.5f * 1.5f * 3.f);
            Assert::IsTrue(CompareEpsilon(s.radius, r));
            Assert::AreEqual(s.center, Vector3(c, c, c));
        }

        TEST_METHOD(TestExtremePointsAlongDirection_OnePoint) {
            Vector3 point(0.f, 0.f, 0.f);
            Vector3 dir(1.f, 2.f, 3.f);
            int min, max;
            tie(min, max) = Harmyder::ExtremePointsAlongDirection(dir, &point, 1);

            Assert::AreEqual(min, 0);
            Assert::AreEqual(max, 0);
        }

        TEST_METHOD(TestExtremePointsAlongDirection_X) {
            Vector3 points[3] = { { 0.f, 0.f, 0.f },{ 0.f, 1.f, 0.f },{ 1.f, 0.f, 0.f }};
            Vector3 dir(1.f, 0.f, 0.f);
            int min, max;
            tie(min, max) = Harmyder::ExtremePointsAlongDirection(dir, points, sizeof(points) / sizeof(Vector3));

            Assert::AreEqual(min, 0);
            Assert::AreEqual(max, 2);
        }
    };
}
