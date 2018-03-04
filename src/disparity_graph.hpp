#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <limits>
#include <stdexcept>

#include "matrix.hpp"

using std::invalid_argument;

/**
 * \brief A graph that stores a pair of given images and possible disparities.
 * The graph consists of nodes, which correspond to pixels of the right image.
 * Labels of the nodes are disparities --- non-negative
 * horizontal displacements of corresponding pixel between two images.
 * They are non-negative, because a pixel from the right image
 * may be only righter on the left image.
 */
template<typename Color> class DisparityGraph {
    private:
        struct Node {
            size_t row;
            size_t column;
            size_t disparity;
        };

        Matrix<Color> leftImage_;
        Matrix<Color> rightImage_;

        void checkNode_(size_t row, size_t column, size_t disparity) const {
            if (row >= this->rightImage_.rows()) {
                throw invalid_argument(
                    "Row should not be greater than the last one.");
            } else if (column >= this->rightImage_.columns()) {
                throw invalid_argument(
                    "Column should not be greater than the last one.");
            } else if (column + disparity >= this->leftImage_.columns()) {
                throw invalid_argument(
                    "Disparity should not lead to image overflow.");
            }
        }
        void checkEdge_(size_t rowA, size_t columnA,
                        size_t rowB, size_t columnB,
                        size_t disparityA, size_t disparityB) const {
            this->checkNode_(rowA, columnA, disparityA);
            this->checkNode_(rowB, columnB, disparityB);
        }
        bool edgeExists_(size_t rowA, size_t columnA, size_t disparityA,
                         size_t rowB, size_t columnB, size_t disparityB) const {
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
        size_t nodeNeighborsCount_(size_t row, size_t column) const {
            return (row > 0) + (row < this->rightImage_.rows())
                + (column > 0) + (column < this->rightImage_.columns());
        }
        double nodePenalty_(size_t row, size_t column, size_t disparity) {
            this->checkNode_(row, column, disparity);
            double difference = static_cast<double>(
                this->rightImage_[row][column])
                - this->leftImage_[row][column + disparity];
            return difference * difference;
        }
        double penalty_(size_t rowA, size_t columnA, size_t disparityA,
                        size_t rowB, size_t columnB, size_t disparityB) {
            this->checkEdge_(rowA, columnA, rowB, columnB,
                             disparityA, disparityB);
            if (!this->edgeExists_(rowA, columnA, disparityA,
                                   rowB, columnB, disparityB)) {
                return std::numeric_limits<double>::infinity();
            }
            double nodesPenalty =
                this->nodePenalty_(rowA, columnA, disparityA)
                    / this->nodeNeighborsCount_(rowA, columnA)
                + this->nodePenalty_(rowB, columnB, disparityB)
                    / this->nodeNeighborsCount_(rowB, columnB);
            double neighboringPenalty = (disparityA - disparityB)
                                      * (disparityA - disparityB);
            return nodesPenalty + neighboringPenalty;
        }
    public:
        DisparityGraph(const DisparityGraph& graph) = default;
        DisparityGraph(DisparityGraph&& graph) = default;
        DisparityGraph(const Matrix<Color>& leftImage,
                       const Matrix<Color>& rightImage)
            : leftImage_{leftImage}
            , rightImage_{rightImage} {
        }
        ~DisparityGraph() = default;
        double nodePenalty(const Node& node) {
            return this->nodePenalty_(node.row, node.column, node.disparity);
        }
        double nodePenalty(size_t row, size_t column, size_t disparity) {
            return this->nodePenalty_(row, column, disparity);
        }
        double penalty(const Node& nodeA, const Node& nodeB) {
            return this->penalty_(nodeA.row, nodeA.column, nodeA.disparity,
                                  nodeB.row, nodeB.column, nodeB.disparity);
        }
};

#endif
