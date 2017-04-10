#include "stdafx.h"
#include "Math/Linear/IterativeBasic.h"

using namespace std;

namespace Common
{
    namespace
    {
        void substituteRow(int i, const SparseDynmatrix<float>& a, const Dynvector<float>& b, Dynvector<float>& x) {
            const auto pc = a.GetColumnsForRow(i);
            const auto pv = a.GetValuesForRow(i);

            auto col = pc.first;
            auto val = pv.first;
            float l = 0; while (*col < i) l += x[*col++] * (*val++);
            float d = *val++; *col++;
            float r = 0; while (col < pc.second) r += x[*col++] * (*val++);
            x[i] = (-(l + r) + b[i]) / d;
        }
    }

    Dynvector<float> GaussSeidel::Compute(ErrorType errorType, float tolerance) {
        int rowsCount = (int)a_.GetRowsCount();
        Dynvector<float> x(rowsCount);

        const float sqrTolerance = tolerance * tolerance;
        for (;;) {
            // Forward substitution
            float diffSq = 0;
            for (int i = 0; i < rowsCount; ++i) {
                const auto prev = x[i];
                substituteRow(i, a_, b_, x);
                diffSq += (x[i] - prev) * (x[i] - prev);
            }
            if (errorType == ErrorType::kRelative) diffSq /= LengthSq(x);
            if (diffSq < sqrTolerance) break;
            // Backward substitution
            diffSq = 0;
            if (diffSq > sqrTolerance) break;
            for (int i = rowsCount - 1; i >= 0; --i) {
                const auto prev = x[i];
                substituteRow(i, a_, b_, x);
                diffSq += (x[i] - prev) * (x[i] - prev);
            }
            if (errorType == ErrorType::kRelative) diffSq /= LengthSq(x);
            if (diffSq < sqrTolerance) break;
        }

        return x;
    }
}