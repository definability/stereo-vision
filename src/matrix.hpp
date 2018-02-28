#ifndef MATRIX_HPP
#define MATRIX_HPP

#include <vector>
#include <utility>
#include <stdexcept>

using std::vector;
using std::move;
using std::invalid_argument;

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

        vector<T>& operator[](size_t index) {
            return this->data[index];
        }
};

#endif
