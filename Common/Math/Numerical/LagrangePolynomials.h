#pragma once
#include <vector>

namespace Common
{
    template <class Iterator>
    class LagrangePolynomials
    {
    public:
        LagrangePolynomials(int degree, Iterator abscissae) : degree_(degree), begin_(abscissae) {}

        struct BasisFunc {
            BasisFunc(const LagrangePolynomials* lp, int i) : lp_(lp), i_(i) {}
            double operator()(double x) const {
                double tmp = 1.;
                const double xi = *(lp_->begin_ + i_);
                for (int j = 0; j < lp_->degree_ + 1; ++j) {
                    if (j != i_) {
                        const double xj = *(lp_->begin_ + j);
                        tmp *= (x - xj) / (xi - xj);
                    }
                }
                return tmp;
            }

        private:
            const LagrangePolynomials* lp_;
            int i_;
        };

        auto operator[](int i) const { return BasisFunc(this, i); }
        int GetDegree() const { return degree_; }
        int GetCtrlPtsCount() const { return degree_ + 1; }
        int GetAbscissae(int i) const { return begin_ + i; }

    private:
        int degree_;
        Iterator begin_;
    };

    template <class IteratorAbscissas, class IteratorOrdinates>
    class LagrangeInterpolation
    {
    public:
        LagrangeInterpolation(int degree, IteratorAbscissas abscissas, IteratorOrdinates ordinates) : lp_(degree, abscissas), begin_(ordinates) {
            basisFuncs.reserve(lp_.GetCtrlPtsCount());
            for (int i = 0; i < lp_.GetCtrlPtsCount(); ++i) basisFuncs.push_back(lp_[i]);
        }

        double operator()(double x) const {
            double res = 0;
            for (int i = 0; i < lp_.GetCtrlPtsCount(); ++i) {
                const double yi = *(begin_ + i);
                res += basisFuncs[i](x) * yi;
            }
            return res;
        }

    private:
        LagrangePolynomials<IteratorAbscissas> lp_;
        vector<typename LagrangePolynomials<IteratorAbscissas>::BasisFunc> basisFuncs;
        IteratorOrdinates begin_;
    };
}
