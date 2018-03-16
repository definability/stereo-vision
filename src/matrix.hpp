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
template<typename T> class Matrix
{
    private:
        /**
         * \brief Number of rows of the matrix.
         */
        size_t rows_;
        /**
         * \brief Number of columns of the matrix.
         */
        size_t columns_;
        /**
         * \brief Storage of matrix elements.
         */
        vector<vector<T> > data_;
    public:
        Matrix() = delete;
        /**
         * \brief Copy matrix in default way.
         */
        Matrix(const Matrix& matrix) = default;
        /**
         * \brief Move matrix in default way.
         */
        Matrix(Matrix&& matrix) = default;
        /**
         * \brief Create a matrix with specified number of rows and columns.
         */
        Matrix(size_t rows, size_t columns)
                : rows_{rows}
                , columns_{columns}
                , data_{vector< vector<T> >(rows)}
        {
            for (vector<T>& row : this->data_)
            {
                row = vector<T>(columns);
            }
        }
        /**
         * \brief Just destroy the matrix and its attributes.
         */
        ~Matrix() = default;
        /**
         * \brief Get number of columns.
         */
        size_t columns() const
        {
            return this->columns_;
        }
        /**
         * \brief Get number of rows.
         */
        size_t rows() const
        {
            return this->rows_;
        }
        /**
         * \brief An ability to read and write colors in particular pixels.
         *
         * Don't rely on a vector type of the output hardly.
         * Use it only to read and write particular pixels,
         * because behavior may change in future.
         */
        vector<T>& operator[](size_t index)
        {
            return this->data_[index];
        }
        /**
         * \brief An ability to read colors in particular pixels.
         *
         * Don't rely on a vector type of the output hardly.
         * Use it only to read particular pixels,
         * because behavior may change in future.
         */
        const vector<T>& operator[](size_t index) const
        {
            return this->data_[index];
        }
};

#endif
