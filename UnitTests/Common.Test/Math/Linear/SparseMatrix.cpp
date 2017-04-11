#include "stdafx.h"
#include "CppUnitTest.h"
#include "Common\Math\Linear\SparseMatrix.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Common;

namespace CommonTest
{		
	TEST_CLASS(SparseMatrix_Test)
	{
        template <class T>
        void build_vectors(
            const vector<vector<T>>& matrix,
            back_insert_iterator<vector<int>> rowIndices,
            back_insert_iterator<vector<int>> colIndices,
            back_insert_iterator<vector<T>> values
        ) {
            for (int i = 0; i < matrix.size(); ++i) {
                assert(matrix[0].size() == matrix[i].size());
                for (int j = 0; j < matrix[0].size(); ++j) {
                    if (matrix[i][j] != 0) {
                        rowIndices++ = i;
                        colIndices++ = j;
                        values++ = matrix[i][j];
                    }
                }
            }
        }

	public:
		
		TEST_METHOD(TestSparseMatrix_Construction) {
            const vector<vector<float>> matrix{
                {11, 0,  0,  0},
                {21, 22, 0,  0},
                {31, 32, 33, 0},
                {41, 42, 43, 44}
            };
            vector<int> rowIndices, colIndices; vector<float> values;
            build_vectors(matrix, back_inserter(rowIndices), back_inserter(colIndices), back_inserter(values));

            SparseDynmatrix<float> sparse(cbegin(rowIndices), cend(rowIndices), cbegin(colIndices), cend(colIndices), cbegin(values), cend(values));

            for (int i = 0; i < matrix.size(); ++i) {
                for (int j = 0; j < matrix[0].size(); ++j) {
                    Assert::AreEqual(matrix[i][j], sparse[i][j]);
                }

                auto pc = sparse.GetColumnsForRow(i);
                auto pv = sparse.GetValuesForRow(i);
                for (int j = 0; j < matrix[i].size(); ++j) {
                    if (matrix[i][j] != 0) {
                        Assert::AreEqual(matrix[i][j], *pv.first++);
                        Assert::AreEqual(j, *pc.first++);
                    }
                }
            }
		}

        TEST_METHOD(TestSparseMatrix_VectorMultiplication) {
            const vector<vector<float>> matrix{
                { 1, 0,  0,  0 },
                { 10, 20, 0,  0 },
                { 100, 200, 300, 0 }
            };
            vector<int> rowIndices, colIndices; vector<float> values;
            build_vectors(matrix, back_inserter(rowIndices), back_inserter(colIndices), back_inserter(values));

            SparseDynmatrix<float> sparse(cbegin(rowIndices), cend(rowIndices), cbegin(colIndices), cend(colIndices), cbegin(values), cend(values), 3, 4);
            const auto vec = sparse * Dynvector<float>(vector<float>(4, 1));

            Assert::AreEqual((size_t)3, vec.size());
            Assert::AreEqual(1.f, vec[0]);
            Assert::AreEqual(30.f, vec[1]);
            Assert::AreEqual(600.f, vec[2]);
        }

        TEST_METHOD(TestSparseMatrix_ConstructionNotSorted) {
            const vector<vector<float>> matrix{ { 1, 2, 3, 4, 5, 6, 7, 8, 9, 0 } };
            vector<int> rowIndices, colIndices; vector<float> values;
            build_vectors(matrix, back_inserter(rowIndices), back_inserter(colIndices), back_inserter(values));
            const auto shift = rowIndices.size() / 2;
            std::rotate(begin(colIndices), begin(colIndices) + shift, end(colIndices));
            std::rotate(begin(values), begin(values) + shift, end(values));

            SparseDynmatrix<float> sparse(cbegin(rowIndices), cend(rowIndices), cbegin(colIndices), cend(colIndices), cbegin(values), cend(values));

            for (int i = 0; i < (int)matrix[0].size(); ++i) {
                Assert::AreEqual(matrix[0][i], sparse[0][i]);
            }
        }
    };
}