#pragma once
// Sparse matrix using Compressed Row Storage (CRS)
#include "Container/Dynarray.h"

namespace Common
{
    template <class T>
    class Dynvector
    {
    public:
        explicit Dynvector(int size) : elems_(size, 0) {}
        explicit Dynvector(const Dynarray<T>& elems) : elems_(elems) {}
        explicit Dynvector(Dynarray<T>&& elems) : elems_(move(elems)) {}
        Dynvector(int size, T value) : elems_(size, value) {}

        auto size() const { return elems_.size(); }

        const T& operator[] (int i) const { return elems_[i]; }
        T& operator[] (int i) { return elems_[i]; }

    private:
        Dynarray<T> elems_;
    };

    template <class T> inline T LengthSq(const Dynvector<T>& v) { T res = 0; for (int i = 0; i < (int)v.size(); ++i) res += v[i] * v[i]; return res; }

    class SparsityPattern
    {
    public:
        SparsityPattern(int rowsCount, int maxEntriesPerRow) :
            maxEntriesPerRow_(maxEntriesPerRow),
            colIndices_(maxEntriesPerRow_ * rowsCount, kInvalidColIndex)
        {}

        int RowsCount() const { return (int)colIndices_.size() / maxEntriesPerRow_; }
        void Add(int rowInd, int colInd);
        bool Exists(int rowInd, int colInd) const;
        bool isSorted() const { return isSorted_; }

        void Sort();
        void Compress(std::back_insert_iterator<std::vector<int>> rowIndices, std::back_insert_iterator<std::vector<int>> colIndices) const;

    private:
        const int kInvalidColIndex = -1;
        int maxEntriesPerRow_;
        Dynarray<int> colIndices_;
        bool isSorted_ = true;
    };

    template <class T>
    class SparseDynmatrix
    {
    public:
        using index_iter = std::vector<int>::const_iterator;
        using value_iter = typename std::vector<T>::const_iterator;
        SparseDynmatrix(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, value_iter bval, value_iter eval, bool isSorted, int rowsCount = -1, int colsCount = -1);
        SparseDynmatrix(const SparsityPattern& pattern);

        class RowProxy {
        public:
            RowProxy(index_iter bc, index_iter ec, value_iter bv) : bc_(bc), ec_(ec), bv_(bv) {}
            T operator[](int j) const {
                auto iter = std::find(bc_, ec_, j);
                return iter == ec_ ? 0 : *(bv_ + (iter - bc_));
            }

        private:
            index_iter bc_, ec_;
            value_iter bv_;
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

        void Set(int i, int j, T value) {
            for (int c = rowOffsets_[i]; c < rowOffsets_[i + 1]; ++c) {
                if (columns_[c] == j) {
                    values_[c] = value;
                    return;
                }
            }
            throw logic_error("This entry can't be non-zero");
        }

        int GetRowsCount() const { return (int)rowOffsets_.size() - 1; }
        int GetColumnsCount() const { return (int)columns_.size(); }

        auto GetColumnsForRow(int r) const { return make_pair(cbegin(columns_) + rowOffsets_[r], cbegin(columns_) + rowOffsets_[r + 1]); }
        auto GetValuesForRow(int r) const { return make_pair(cbegin(values_) + rowOffsets_[r], cbegin(values_) + rowOffsets_[r + 1]); }

    private:
        void Init(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, const std::function<T(int)>& val, bool isSorted);

    private:
        const int rowsCount_, colsCount_;
        std::vector<int> rowOffsets_;
        std::vector<int> columns_;
        std::vector<T> values_;
    };

    template <class T>
    SparseDynmatrix<T>::SparseDynmatrix(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, value_iter bval, value_iter eval, bool isSorted, int rowsCount, int colsCount) :
        rowsCount_(rowsCount == -1 ? *max_element(brow, erow) + 1 : rowsCount),
        colsCount_(colsCount == -1 ? *max_element(bcol, ecol) + 1 : colsCount),
        rowOffsets_(rowsCount_ + 1, 0) // one for guard
    {
        assert(erow - brow == ecol - bcol);
        Init(brow, erow, bcol, ecol, [bval](int i) { return *(bval + i); }, isSorted);
    }

    template <class T>
    void SparseDynmatrix<T>::Init(index_iter brow, index_iter erow, index_iter bcol, index_iter ecol, const std::function<T(int)>& val, bool isSorted) {
        assert(erow - brow == ecol - bcol);
        for (auto i = brow; i < erow; ++i) ++rowOffsets_[*i]; // numbers of elements in every row
        // From numbers of elements per row to offsets
        auto currentOffset = 0;
        for (auto i = begin(rowOffsets_); i < end(rowOffsets_) - 1; ++i) {
            auto elemsInRow = *i;
            if (elemsInRow == 0) throw logic_error("Can't have empty rows");
            *i = currentOffset;
            currentOffset += elemsInRow;
        }

        const int nonZerosCount = (int)(ecol - bcol);
        values_.resize(nonZerosCount);
        columns_.resize(nonZerosCount);
        auto currentCol = bcol;
        auto currentRow = brow;
        for (int v = 0; v < nonZerosCount; ++v, ++currentCol, ++currentRow) {
            columns_[rowOffsets_[*currentRow]] = *currentCol;
            values_[rowOffsets_[*currentRow]++] = val(v);
        }
        std::rotate(rbegin(rowOffsets_), rbegin(rowOffsets_) + 1, rend(rowOffsets_));

        // Sort columns/values for every row in case data was passed not sorted
        if (!isSorted) {
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

    template <class T>
    SparseDynmatrix<T>::SparseDynmatrix(const SparsityPattern& pattern) :
        rowsCount_(pattern.RowsCount()),
        colsCount_(rowsCount_),
        rowOffsets_(rowsCount_ + 1, 0) // one for guard
    {
        vector<int> rowsIndices, colsIndices;
        pattern.Compress(back_inserter(rowsIndices), back_inserter(colsIndices));
        Init(begin(rowsIndices), end(rowsIndices), begin(colsIndices), end(colsIndices), [](int i) { return T(); }, pattern.isSorted());
    }
}
