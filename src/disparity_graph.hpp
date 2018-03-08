#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <cassert>
#include <limits>
#include <stdexcept>
#include <utility>

#include "matrix.hpp"

using std::invalid_argument;
using std::numeric_limits;
using std::swap;

/**
 * \brief An information that uniquely identifies a node like coordinates.
 */
struct DisparityNode {
    size_t row;
    size_t column;
    size_t disparity;
};

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
        Matrix<Color> leftImage_;
        Matrix<Color> rightImage_;

        /**
         * \brief Check that the node exists in this graph.
         *
         * Node coordinates should not be out of the right image.
         * Disparity should not lead out of the left image.
         */
        void checkNode_(const DisparityNode& node) const {
            if (node.row >= this->rightImage_.rows()) {
                throw invalid_argument(
                    "Row should not be greater than the last one.");
            } else if (node.column >= this->rightImage_.columns()) {
                throw invalid_argument(
                    "Column should not be greater than the last one.");
            } else if (node.column + node.disparity
                       >= this->leftImage_.columns()) {
                throw invalid_argument(
                    "Disparity should not lead to image overflow.");
            }
        }
        /**
         * \brief Check that given nodes may be connected by an edge
         * in the graph.
         *
         * Both nodes should exist.
         *
         * A pixel cannot be a naighbor of itself.
         */
        void checkEdge_(const DisparityNode& nodeA,
                        const DisparityNode& nodeB) const {
            if (nodeA.column == nodeB.column && nodeA.row == nodeB.row) {
                throw invalid_argument(
                    "A pixel cannot be connected with itself.");
            }
            this->checkNode_(nodeA);
            this->checkNode_(nodeB);
        }
        /**
         * \brief Calculate number of neighbor nodes of the given one.
         *
         * There are four possible neighbors:
         * left, right, top and bottom.
         *
         * If the pixel of located in a corder or on a border,
         * it obviously has less number of neighbors.
         */
        size_t nodeNeighborsCount_(const DisparityNode& node) const {
            return
                (node.row > 0)
                + (node.row < this->rightImage_.rows())
                + (node.column > 0)
                + (node.column < this->rightImage_.columns());
        }
    public:
        DisparityGraph(const DisparityGraph& graph) = default;
        DisparityGraph(DisparityGraph&& graph) = default;
        DisparityGraph(const Matrix<Color>& leftImage,
                       const Matrix<Color>& rightImage)
                : leftImage_{leftImage}
                , rightImage_{rightImage} {
            if (leftImage.columns() != rightImage.columns()) {
                throw invalid_argument(
                    "Images should have the same number of columns.");
            }
        }
        ~DisparityGraph() = default;
        /**
         * \brief Calculate a penalty of the edge between two given nodes.
         *
         * If nodes have wrong position and/or disparity,
         * an exception will occur.
         * If nodes are not connected, the weight is infinity.
         *
         * Otherwise, the penalty is a sum of
         * squared difference of disparities
         * and penalties of nodes divided by number of their neighbors.
         */
        double penalty(const DisparityNode& nodeA,
                       const DisparityNode& nodeB) {
            if (!this->edgeExists(nodeA, nodeB)) {
                return numeric_limits<double>::infinity();
            }
            double nodesPenalty =
                this->nodePenalty(nodeA) / this->nodeNeighborsCount_(nodeA)
                + this->nodePenalty(nodeB) / this->nodeNeighborsCount_(nodeB);
            double neighboringPenalty = (nodeA.disparity - nodeB.disparity)
                                      * (nodeA.disparity - nodeB.disparity);
            return nodesPenalty + neighboringPenalty;
        }
        /**
         * \brief Check that given nodes are connected by an edge
         * in the graph.
         *
         * Only nearest pixels may be connected.
         * This means, that two nodes should differ
         * eigher by column or by row, no both at the same time.
         *
         * If a pixel of the right image
         * is located to the right of its neighbor,
         * it can point to either the same pixel as its neighbor
         * or at the righter one --- it cannot correspond to a pixel
         * that is to the left of corresponding pixel of its left neighbor.
         */
        bool edgeExists(const DisparityNode& nodeA,
                        const DisparityNode& nodeB) const {
            this->checkEdge_(nodeA, nodeB);

            if (nodeA.row != nodeB.row && nodeA.column != nodeB.column) {
                return false;
            }

            size_t topRow = nodeA.row;
            size_t bottomRow = nodeB.row;
            if (topRow < bottomRow) {
                swap(topRow, bottomRow);
            }
            if (bottomRow - topRow > 1) {
                return false;
            }

            size_t leftColumn = nodeA.column;
            size_t leftDisparity = nodeA.disparity;
            size_t rightColumn = nodeB.column;
            size_t rightDisparity = nodeB.disparity;
            if (leftColumn > rightColumn) {
                swap(leftColumn, rightColumn);
                swap(leftDisparity, rightDisparity);
            }
            if (rightColumn - leftColumn > 1
                    || rightDisparity + 1 < leftDisparity) {
                return false;
            }

            return true;
        }
        /**
         * \brief Calculate a penalty of the node.
         *
         * The penalty is a Euclidian norm of a difference
         * between color of the pixel of the right image
         * and corresponding imagge of the left image.
         */
        double nodePenalty(const DisparityNode& node) {
            this->checkNode_(node);
            double difference = static_cast<double>(
                this->rightImage_[node.row][node.column])
                - this->leftImage_[node.row][node.column + node.disparity];
            return difference * difference;
        }
        double nodePenalty(size_t row, size_t column, size_t disparity) {
            return this->nodePenalty({row, column, disparity});
        }
};

#endif
