#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common\Math\Linear\IterativeBasic.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{
    TEST_CLASS(SparseMatrix)
    {
        auto BuildSparseMatrix(vector<vector<float>> matrix) {
            vector<int> rows, cols;
            vector<float> values;
            for (int i = 0; i < (int)matrix.size(); ++i) {
                for (int j = 0; j < (int)matrix[i].size(); ++j) {
                    if (matrix[i][j] != 0) {
                        rows.push_back(i);
                        cols.push_back(j);
                        values.push_back(matrix[i][j]);
                    }
                }
            }
            return SparseDynmatrix<float>(cbegin(rows), cend(rows), cbegin(cols), cend(cols), cbegin(values), cend(values));
        }

        void AssertSolution(const Dynvector<float>& expected, const Dynvector<float>& actual, GaussSeidel::ErrorType errorType, float tolerance) {
            Assert::AreEqual(expected.size(), actual.size());
            float expectedNormSq = 0;
            float actualNormSq = 0;
            for (int i = 0; i < (int)expected.size(); ++i) {
                expectedNormSq += expected[i] * expected[i];
                actualNormSq += actual[i] * actual[i];
            }
            const float relErrorSqr = abs(expectedNormSq - actualNormSq) / expectedNormSq;
            Assert::IsTrue(relErrorSqr < tolerance * tolerance);
        }

    public:

        TEST_METHOD(TestIterativeBasic_GaussSeidel) {
            vector<vector<float>> matrix = {
                {  2, -1,  0,  0 },
                { -1,  2, -1,  0 },
                {  0, -1,  2, -1 },
                {  0,  0,- 1,  2 }
            };
            const auto a = BuildSparseMatrix(matrix);
            const auto b = Dynvector<float>(a.GetRowsCount(), 1.f);

            GaussSeidel gs(a, b);
            const float tolerance = 0.001f;
            const auto x = gs.Compute(GaussSeidel::ErrorType::kRelative, tolerance);

            // toeplitz([2 -1 0 0])\[1 1 1 1]
            const auto res = Dynvector<float>(vector<float>{2.f, 3.f, 3.f, 2.f});
            const float solution_tolerance = 0.1f;
            AssertSolution(res, x, GaussSeidel::ErrorType::kRelative, solution_tolerance);
        }
    };
}