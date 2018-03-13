#ifndef DISPARITY_GRAPH_HPP
#define DISPARITY_GRAPH_HPP

#include <cassert>
#include <limits>
#include <stdexcept>
#include <utility>
#include <vector>

#include "matrix.hpp"

using std::invalid_argument;
using std::numeric_limits;
using std::swap;
using std::vector;

/**
 * \brief An information that uniquely identifies a node like coordinates.
 */
struct DisparityNode {
    /**
     * \brief Row on which the node located.
     *
     * \f[
     *  t_x \in \mathbb{N}_0.
     * \f]
     */
    size_t row;
    /**
     * \brief Column on which the node located.
     *
     * \f[
     *  t_y \in \mathbb{N}_0.
     * \f]
     */
    size_t column;
    /**
     * \brief Disparity is an offset
     * between the pixel on a right image
     * and corresponding one on a left image.
     *
     * \f[
     *  k \in \mathbb{N}_0.
     * \f]
     */
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
        /**
         * \brief Matrix representing left image.
         */
        Matrix<Color> leftImage_;
        /**
         * \brief Matrix representing right image.
         */
        Matrix<Color> rightImage_;
        /**
         * \brief Multiplier for edge penalty.
         *
         * \f[
         *  \alpha \ge 0.
         * \f]
         */
        double consistency_;
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
            this->checkNode(nodeA);
            this->checkNode(nodeB);
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
        size_t nodeNeighborsCount_(const DisparityNode& node,
                                   bool directed = false) const {
            return
                (node.row > 0 && !directed)
                + (node.row < this->rightImage_.rows() - 1)
                + (node.column > 0 && !directed)
                + (node.column < this->rightImage_.columns() - 1);
        }
    public:
        DisparityGraph() = delete;
        /**
         * \brief Copy graph in default way.
         */
        DisparityGraph(const DisparityGraph&) = default;
        /**
         * \brief Move graph in default way.
         */
        DisparityGraph(DisparityGraph&&) = default;
        /**
         * \brief A disparity graph needs two images --- left and right one.
         *
         * Input images should be made by equal cameras
         * that look at the same direction
         * (normal vectors to planes of their films should be equal)
         * differing only by horizontal offset
         * (\f$x\f$ coordinate, a column number).
         * That's why each row of one image corresponds to the same row
         * of another image and number of rows should be equal.
         * Number of columns is not checked,
         * so images could be trimmed if it's needed.
         */
        DisparityGraph(const Matrix<Color>& leftImage,
                       const Matrix<Color>& rightImage,
                       double consistency = 1)
                : leftImage_{leftImage}
                , rightImage_{rightImage}
                , consistency_{consistency} {
            if (!rightImage.rows() || !rightImage.columns()) {
                throw invalid_argument(
                    "Images should contain at least one pixel.");
            }
            if (leftImage.rows() != rightImage.rows()) {
                throw invalid_argument(
                    "Images should have the same number of rows.");
            }
            if (leftImage.columns() < rightImage.columns()) {
                throw invalid_argument(
                    "Left image should have at least as much columns "
                    "as the right one.");
            }
            if (consistency_ < 0) {
                throw invalid_argument(
                    "Consistency term cannot be lower than 0.");
            }
        }
        /**
         * \brief Just destroy the graph and its copies of images.
         */
        ~DisparityGraph() = default;
        /**
         * \brief Calculate a penalty of the edge between two given nodes.
         *
         * If nodes have wrong position and/or disparity,
         * an exception will occur.
         * If nodes are not connected, the weight is infinity.
         *
         * \f[
         *  g_{tt'}\left( k, k' \right ) = \infty, \qquad tt' \notin \tau.
         * \f]
         *
         * Otherwise, the penalty is a sum of
         * squared difference of disparities
         * and penalties of nodes divided by number of their neighbors
         *
         * \f[
         *  g_{tt'}\left( k, k' \right )
         *      = \alpha \cdot \left( k - k' \right)^2
         *      + \frac{q_t\left( k \right)}
         *             {\left| N\left( t \right ) \right|}
         *      + \frac{q_t'\left( k' \right)}
         *             {\left| N\left( t' \right ) \right|},
         *      \qquad tt' \in \tau,
         *      \qquad N\left( t \right)
         *      = \left\{ t' \;\middle|\; tt' \in \tau \right\}.
         * \f]
         */
        double penalty(const DisparityNode& nodeA,
                       const DisparityNode& nodeB) const {
            if (!this->edgeExists(nodeA, nodeB)) {
                return numeric_limits<double>::infinity();
            }
            double nodesPenalty =
                this->nodePenalty(nodeA) / this->nodeNeighborsCount_(nodeA)
                + this->nodePenalty(nodeB) / this->nodeNeighborsCount_(nodeB);
            double neighboringPenalty = (nodeA.disparity - nodeB.disparity)
                                      * (nodeA.disparity - nodeB.disparity);
            return nodesPenalty + this->consistency_ * neighboringPenalty;
        }
        /**
         * \brief Check that the node exists in this graph.
         *
         * Node coordinates should not be out of the right image.
         * Disparity should not lead out of the left image.
         */
        void checkNode(const DisparityNode& node) const {
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
         * \brief Get all available nodes with zero disparities.
         *
         * Zero disparities cannot conflict with each other,
         * so this may be a good start for an optimization procedure
         */
        vector<DisparityNode> availableNodes() const {
            vector<DisparityNode> result(
                this->rightImage_.rows() * this->rightImage_.columns());

            size_t rows = this->rightImage_.rows();
            size_t columns = this->rightImage_.columns();

            for (size_t row = 0; row < rows; ++row) {
                for (size_t column = 0; column < columns; ++column) {
                    result[row * columns + column] = {row, column};
                }
            }
            return result;
        }
        /**
         * \brief Get neighbor nodes with zero disparities.
         */
        vector<DisparityNode> nodeNeighbors(const DisparityNode& node,
                                            bool directed = false) const {
            this->checkNode(node);
            vector<DisparityNode> result;

            if (node.column < this->rightImage_.columns() - 1) {
                result.push_back({node.row, node.column + 1});
            }
            if (node.row < this->rightImage_.rows() - 1) {
                result.push_back({node.row + 1, node.column});
            }

            if (directed) {
                assert(result.size() == this->nodeNeighborsCount_(node, true));
                return result;
            }

            if (node.column > 0) {
                result.push_back({node.row, node.column - 1});
            }
            if (node.row > 0) {
                result.push_back({node.row - 1, node.column});
            }

            assert(result.size() == this->nodeNeighborsCount_(node));
            return result;
        }
        /**
         * \brief Get available disparities of the neighbor of given node.
         */
        vector<size_t> neighborDisparities(
                const DisparityNode& node, DisparityNode neighbor)
                const {
            vector<size_t> result;
            size_t columns = this->rightImage_.columns();
            neighbor.disparity = 0;

            this->checkNode(node);
            this->checkNode(neighbor);

            if (node.row != neighbor.row
                    && !this->edgeExists(node, neighbor)) {
                return result;
            } else if (node.row != neighbor.row) {
                for (size_t disparity = 0;
                        neighbor.column + disparity < columns; ++disparity) {
                    result.push_back(disparity);
                }
                return result;
            } else if (node.row == neighbor.row
                    && node.column == neighbor.column + 1) {
                for (size_t disparity = 0; disparity <= node.disparity + 1;
                        ++disparity) {
                    result.push_back(disparity);
                }
                return result;
            } else if (node.row == neighbor.row
                    && node.column + 1 == neighbor.column) {
                for (size_t disparity = neighbor.disparity
                            ? neighbor.disparity - 1
                            : 0;
                        neighbor.column + disparity < columns;
                        ++disparity) {
                    result.push_back(disparity);
                }
                return result;
            } else {
                return result;
            }
        }
        /**
         * \brief Check that given nodes are connected by an edge
         * in the graph.
         *
         * Only nearest pixels may be connected.
         * This means, that two nodes should differ
         * eigher by column or by row, no both at the same time
         *
         * \f[
         *  \left| t_x - t'_x \right| + \left| t_y - t'_y \right| \neq 1
         *      \Rightarrow tt' \notin \tau.
         * \f]
         *
         * If a pixel of the right image
         * is located to the right of its neighbor,
         * it can point to either the same pixel as its neighbor
         * or at the righter one --- it cannot correspond to a pixel
         * that is to the left of corresponding pixel of its left neighbor
         *
         * \f{eqnarray*}{
         *  t_x < t'_x,\; t_x + k > t'_x + k'
         *      \Rightarrow tt' \notin \tau, \\
         *  t_x > t'_x,\; t_x + k < t'_x + k'
         *      \Rightarrow tt' \notin \tau.
         * \f}
         */
        bool edgeExists(const DisparityNode& nodeA,
                        const DisparityNode& nodeB) const {
            this->checkEdge_(nodeA, nodeB);

            if (nodeA.row != nodeB.row && nodeA.column != nodeB.column) {
                return false;
            }

            size_t topRow = nodeA.row;
            size_t bottomRow = nodeB.row;
            if (topRow > bottomRow) {
                swap(topRow, bottomRow);
            }
            if (topRow + 1 < bottomRow) {
                return false;
            } else if (nodeA.column == nodeB.column) {
                return true;
            }

            size_t leftColumn = nodeA.column;
            size_t leftDisparity = nodeA.disparity;
            size_t rightColumn = nodeB.column;
            size_t rightDisparity = nodeB.disparity;
            if (leftColumn > rightColumn) {
                swap(leftColumn, rightColumn);
                swap(leftDisparity, rightDisparity);
            }
            if (leftColumn + 1 < rightColumn
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
         * and corresponding imagge of the left image
         *
         * \f[
         *  q_t\left( k \right)
         *  = \left\| \pmb{l}\left( t_x + k, t_y \right)
         *          - \pmb{r}\left( t_x, t_y \right) \right\|^2.
         * \f]
         */
        double nodePenalty(const DisparityNode& node) const {
            this->checkNode(node);
            double difference = static_cast<double>(
                this->rightImage_[node.row][node.column])
                - this->leftImage_[node.row][node.column + node.disparity];
            return difference * difference;
        }
        /**
         * @copydoc DisparityGraph::nodePenalty(const DisparityNode& node)
         */
        double nodePenalty(size_t row, size_t column, size_t disparity) const {
            return this->nodePenalty({row, column, disparity});
        }
        /**
         * \brief Get number of columns of the right image.
         *
         * Number of columns of the right image
         * is a number of horizontal nodes.
         */
        size_t columns() const {
            return this->rightImage_.columns();
        }
        /**
         * \brief Get number of rows of the right (and left) image.
         *
         * Number of rows of the right image
         * is a number of vertical nodes
         * and is equal to the number of rows of the left image.
         */
        size_t rows() const {
            return this->rightImage_.rows();
        }
};

#endif
