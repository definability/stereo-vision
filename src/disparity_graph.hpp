#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <limits>
#include <stdexcept>

#include "matrix.hpp"

using std::invalid_argument;

template<typename Color> class DisparityGraph {
    private:
        Matrix<Color> leftImage_;
        Matrix<Color> rightImage_;
    private:
        void checkEdge_(size_t rowA, size_t columnA,
                        size_t rowB, size_t columnB,
                        int disparityA, int disparityB) const {
            if (rowA >= this->leftImage_.rows()) {
                throw invalid_argument();
            } else if (columnA >= this->leftImage_.columns_()) {
                throw invalid_argument();
            } else if (rowA >= this->leftImage_.rows()) {
                throw invalid_argument();
            } else if (columnA >= this->leftImage_.columns_()) {
                throw invalid_argument();
            } else if (columnA + disparityA >= this->rightImage_.columns_()) {
                throw invalid_argument();
            } else if (columnB + disparityB >= this->rightImage_.columns_()) {
                throw invalid_argument();
            }
        }
    public:
        DisparityGraph(const DisparityGraph& graph) = default;
        DisparityGraph(DisparityGraph&& graph) = default;
        ~DisparityGraph() = default;
        bool edgeExists(size_t rowA, size_t columnA,
                        size_t rowB, size_t columnB,
                        int disparityA, int disparityB) const {
            this->checkEdge_(rowA, columnA, rowB, columnB,
                             disparityA, disparityB);

            if (rowA != rowB && columnA != columnB) {
                return false;
            } else if (rowA == rowB
                    && (columnA != columnB + 1 && columnA != columnB - 1)) {
                return false;
            } else if (columnA == columnB
                    && (rowA != rowB + 1 && rowA != rowB - 1)) {
                return false;
            } else if (rowA == rowB) {
                if (columnA == columnB - 1 && disparityB < disparityA - 1) {
                    return false;
                } else if (columnB == columnA - 1
                        && disparityA < disparityB - 1) {
                    return false;
                } else {
                    return true;
                }
            } else {
                return true;
            }
        }
        double penalty(size_t rowA, size_t columnA,
                        size_t rowB, size_t columnB,
                        int disparityA, int disparityB) const {
            this->checkEdge_(rowA, columnA, rowB, columnB,
                             disparityA, disparityB);
            if (!this->edgeExists(rowA, columnA, rowB, columnB,
                                  disparityA, disparityB)) {
                return std::numeric_limits<double>::infinity();
            }
            throw;
        }
};

#endif
