#pragma once
// Defines some widely used matrices in inefficient way just for testing purposes

namespace _Utils
{
    template <class T>
    std::vector<std::vector<T>> toeplitz(const std::vector<T>& firstCol, const std::vector<T>& firstRow) {
        const auto rowsCount = firstCol.size();
        const auto colsCount = firstRow.size() == 0 ? rowsCount : firstRow.size() ;
        std::vector<std::vector<T>> matrix(rowsCount, std::vector<T>(colsCount));
        if (firstRow.size() > 0 && firstCol[0] != firstRow[0]) throw logic_error("First elements of first row and first column do not match");
        const auto& innerFirstRow = firstRow.size() == 0 ? firstCol : firstRow;
        for (int i = 0; i < (int)rowsCount; ++i) {
            for (int j = 0; j < min(i, (int)colsCount); ++j) matrix[i][j] = firstCol[i - j];
            for (int j = i; j < (int)colsCount; ++j) matrix[i][j] = innerFirstRow[j - i];
        }
        return matrix;
    }

    template <class T>
    std::vector<std::vector<T>> toeplitz(const std::vector<T>& c) {
        return toeplitz(c, {});
    }
}