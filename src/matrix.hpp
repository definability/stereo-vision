#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <stdexcept>

using std::vector;
using std::invalid_argument;

/**
 * \brief The Matrix is a storage for pixels of images.
 *
 * Images may be grayscale,
 * contain RGB or RGBA color channels,
 * that's why a template parameter T is needed.
 */
template<typename T> class Matrix {
    private:
        vector<vector<T> > data;
        size_t rows_;
        size_t columns_;
    public:
        Matrix(const Matrix& matrix) = default;
        Matrix(Matrix&& matrix) = default;
        Matrix(size_t rows, size_t columns) {
            this->rows_ = rows;
            this->columns_ = columns;
            this->data = vector< vector<T> >(rows);
            for (vector<T>& row : this->data) {
                row = vector<T>(columns);
            }
        }
        ~Matrix() = default;

        size_t columns() const {
            return this->columns_;
        }

        size_t rows() const {
            return this->rows_;
        }

        /**
         * \brief An ability to read and write colors in particular pixels.
         *
         * Don't rely on a vector type of the output hardly.
         * Use it only to read and write particular pixels,
         * because behavior may change in future.
         */
        vector<T>& operator[](size_t index) {
            return this->data[index];
        }
};

#endif
