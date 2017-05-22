#pragma once
// Implementation of symmetric Gauss-Seidel iteration
// Convergence of Gauss-Seidel only ensured if matrix is diagonally dominant i.e.
//     |a_{i,i}| >= \sum_{j=1,i=\=j}^n |a_{i,j}|, for all i
//     and inequality is strict for at least one i
// otherwise convergence isn't guaranteed

#include "Math/Linear/SparseMatrix.h"

namespace Common
{
    class GaussSeidel
    {
    public:
        GaussSeidel::GaussSeidel(const SparseDynmatrix<float>& a, const Dynvector<float>& b) : a_(a), b_(b) {}

        enum class ErrorType { kRelative, kAbsolute };
        Dynvector<float> Compute(ErrorType errorType, float tolerance);

    private:

        const SparseDynmatrix<float>& a_;
        const Dynvector<float>& b_;
    };
}
