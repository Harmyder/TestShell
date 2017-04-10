#pragma once
// Implementation of symmetric Gauss-Seidel iteration

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
