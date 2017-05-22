#include "stdafx.h"
#include "SparseMatrix.h"

using namespace std;

namespace Common
{
    void SparsityPattern::Add(int rowInd, int colInd) {
        const int rowStart = rowInd * maxEntriesPerRow_;
        for (int i = rowStart; i < rowStart + maxEntriesPerRow_; ++i) {
            if (colIndices_[i] == colInd) return;
            if (colIndices_[i] == kInvalidColIndex) {
                colIndices_[i] = colInd;
                if (i != rowStart && colIndices_[i - 1] > colInd) isSorted_ = false;
                return;
            }
        }
    }

    bool SparsityPattern::Exists(int rowInd, int colInd) const {
        const int rowStart = rowInd * maxEntriesPerRow_;
        for (int i = rowStart; i < rowStart + maxEntriesPerRow_; ++i) {
            if (colIndices_[i] == colInd) return true;
        }
        return false;
    }

    void SparsityPattern::Sort() {
        for (int rowStart = 0; rowStart < (int)colIndices_.size(); rowStart += maxEntriesPerRow_) {
            int i = 0;
            while (colIndices_[rowStart + i] != kInvalidColIndex) ++i;
            sort(begin(colIndices_) + rowStart, begin(colIndices_) + rowStart + i);
        }
    }

    void SparsityPattern::Compress(std::back_insert_iterator<std::vector<int>> rowIndices, std::back_insert_iterator<std::vector<int>> colIndices) const {
        for (int rowStart = 0; rowStart < (int)colIndices_.size(); rowStart += maxEntriesPerRow_) {
            const int currentRow = rowStart / maxEntriesPerRow_;
            auto i = rowStart;
            while (colIndices_[i] != kInvalidColIndex && i < rowStart + maxEntriesPerRow_) {
                rowIndices++ = currentRow;
                colIndices++ = colIndices_[i++];
            }
        }
    }
}
