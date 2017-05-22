#include "stdafx.h"
#include "CppUnitTest.h"
#include "_Utils/Matrices.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace _Utils
{
    namespace _UtilsTest
    {
        TEST_CLASS(Utils_Matrices_Test)
        {
            template <class T>
            void AssertStrictEquality(const vector<vector<T>>& matrixExpected, const vector<vector<T>>& matrixActual) {
                Assert::AreEqual(matrixExpected.size(), matrixActual.size());
                for (int i = 0; i < (int)matrixExpected.size(); ++i) {
                    Assert::AreEqual(matrixExpected[i].size(), matrixActual[i].size());
                    for (int j = 0; j < (int)matrixExpected[i].size(); ++j) {
                        Assert::AreEqual(matrixExpected[i][j], matrixActual[i][j]);
                    }
                }
            }

        public:

            TEST_METHOD(_TestMatrices_Symmetric) {
                const auto matrix = toeplitz(vector<int>{ 1, 2, 3 });
                const vector<vector<int>> matrixExpected = {
                    { 1, 2, 3 },
                    { 2, 1, 2 },
                    { 3, 2, 1 },
                };
                AssertStrictEquality(matrixExpected, matrix);
            }

            TEST_METHOD(_TestMatrices_Nonsymmetric) {
                const auto matrix = toeplitz(vector<int>{ 1, 2, 3 }, vector<int>{1, 4, 5});
                const vector<vector<int>> matrixExpected = {
                    { 1, 4, 5 },
                    { 2, 1, 4 },
                    { 3, 2, 1 },
                };
                AssertStrictEquality(matrixExpected, matrix);
            }

            TEST_METHOD(_TestUtilsMatrices_Nonsquare) {
                const auto matrix = toeplitz(vector<int>{ 1, 2, 3 }, vector<int>{1});
                const vector<vector<int>> matrixExpected = { {1}, {2}, {3} };
                AssertStrictEquality(matrixExpected, matrix);
            }

            TEST_METHOD(_TestUtilsMatrices_Donotmatch) {
                Assert::ExpectException<logic_error>([]() { toeplitz(vector<int>{ 1 }, vector<int>{2}); });
            }
        };
    }
}
