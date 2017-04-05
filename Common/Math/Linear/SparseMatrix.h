#pragma once
// Sparse matrix using Compressed Row Storage (CRS)

namespace Common
{
    template <class T>
    class Dynvector
    {
    public:
        explicit Dynvector(int size) : elems_(size, 0) {}
        explicit Dynvector(const std::vector<T>& elems) : elems_(elems) {}
        explicit Dynvector(std::vector<T>&& elems) : elems_(move(elems)) {}

        auto size() const { return elems_.size(); }

        const T& operator[] (int i) const { return elems_[i]; }
        T& operator[] (int i) { return elems_[i]; }

    private:
        std::vector<T> elems_;
    };

    template <class T>
    class SparseDynmatrix
    {
    public:
        using index_iter = std::vector<int>::const_iterator;
        using value_iter = typename std::vector<T>::const_iterator;
        SparseDynmatrix(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, value_iter bval, value_iter eval, int rowsCount = -1, int colsCount = -1);

        using ColValues = std::vector<std::pair<int, T>>;
        using ColumnsConstIterator = typename std::vector<int>::const_iterator;
        using ValuesConstIterator = typename std::vector<T>::const_iterator;

        class RowProxy {
        public:
            RowProxy(ColumnsConstIterator bc, ColumnsConstIterator ec, ValuesConstIterator bv) : bc_(bc), ec_(ec), bv_(bv) {}
            T operator[](int j) const {
                auto iter = std::find(bc_, ec_, j);
                return iter == ec_ ? 0 : *(bv_ + (iter - bc_));
            }

        private:
            ColumnsConstIterator bc_, ec_;
            ValuesConstIterator bv_;
        };

        RowProxy operator[] (int i) const {
            auto bc = begin(columns_) + rowOffsets_[i];
            auto ec = begin(columns_) + rowOffsets_[i + 1];
            auto bv = begin(values_) + rowOffsets_[i];
            return RowProxy(bc, ec, bv);
        }

        Dynvector<T> operator* (Dynvector<T> vec) const {
            assert(vec.size() == colsCount_);
            Dynvector<T> res(rowsCount_);
            for (int i = 0; i < rowsCount_; ++i) {
                auto currentValIndex = rowOffsets_[i];
                res[i] = 0;
                for (int j = 0; j < colsCount_; ++j) {
                    if (columns_.size() > currentValIndex && columns_[currentValIndex] == j) {
                        res[i] += values_[currentValIndex] * vec[j];
                        ++currentValIndex;
                    }
                }
            }
            return res;
        }

        auto GetColumnsForRow(int rowIndex) { return make_pair(begin(columns_) + rowOffsets_[i], begin(columns_) + rowOffsets_[i + 1]); }

    private:
        std::vector<int> rowOffsets_;
        std::vector<int> columns_;
        std::vector<T> values_;
        int rowsCount_, colsCount_;
    };

    template <class T>
    SparseDynmatrix<T>::SparseDynmatrix(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, value_iter bval, value_iter eval, int rowsCount, int colsCount) {
        assert(erow - brow == ecol - bcol && erow - brow == eval - bval);
        rowsCount_ = rowsCount == -1 ? *max_element(brow, erow) + 1 : rowsCount;
        colsCount_ = colsCount == -1 ? *max_element(bcol, ecol) + 1 : colsCount;

        rowOffsets_.resize(rowsCount_ + 1, 0); // one for guard
        for (auto i = brow; i < erow; ++i) ++rowOffsets_[*i];
        auto currentOffset = 0;
        for (auto i = begin(rowOffsets_); i < end(rowOffsets_) - 1; ++i) {
            auto tmp = *i;
            *i = currentOffset;
            currentOffset += tmp;
        }

        values_.resize(eval - bval);
        columns_.resize(ecol - bcol);
        auto currentCol = bcol;
        auto currentRow = brow;
        for (auto v = bval; v < eval; ++v, ++currentCol, ++currentRow) {
            columns_[rowOffsets_[*currentRow]] = *currentCol;
            values_[rowOffsets_[*currentRow]++] = *v;
        }
        std::rotate(rbegin(rowOffsets_), rbegin(rowOffsets_) + 1, rend(rowOffsets_));

        std::vector<std::pair<int, T>> tmp;
        for (auto r = begin(rowOffsets_); r < end(rowOffsets_) - 1; ++r) {
            auto b = begin(columns_) + *r;
            auto e = begin(columns_) + *(r + 1);
            if (!std::is_sorted(b, e)) {
                auto iterv = begin(values_);
                for (auto iterc = b; iterc < e; ++iterc, ++iterv) {
                    tmp.emplace_back(*iterc, *iterv);
                }
                std::sort(begin(tmp), end(tmp));
                std::transform(cbegin(tmp), cend(tmp), begin(columns_), [](const auto& el) { return el.first; });
                std::transform(cbegin(tmp), cend(tmp), begin(values_), [](const auto& el) { return el.second; });
                tmp.clear();
            }
        }
    }
}
