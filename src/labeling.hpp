#ifndef LABELING_HPP
#define LABELING_HPP

#include <algorithm>
#include <cmath>
#include <limits>
#include <set>
#include <vector>

#include "disparity_graph.hpp"

using std::find;
using std::isfinite;
using std::multiset;
using std::numeric_limits;
using std::vector;

/**
 * \brief Labeling is a set of labeled pixels.
 * Needed to store particular labeling,
 * change it without making infinite penalty
 * and calculate the penalty.
 */
template<typename Color> class Labeling {
    private:
        /**
         * \brief The basic problem descriptor.
         */
        const DisparityGraph<Color>& graph_;
        /**
         * \brief Labeled nodes.
         */
        vector<DisparityNode> nodes_;
        /**
         * \brief Cached value of the penalty.
         */
        double penalty_ = numeric_limits<double>::infinity();
        /**
         * \brief Get constant iterator to the node;
         */
        vector<DisparityNode>::const_iterator nodeIterator_(
                const DisparityNode& node) const {
            return this->nodes_.begin()
                + node.row * this->graph_.columns() + node.column;
        }
        /**
         * \brief Get iterator to the node;
         */
        vector<DisparityNode>::iterator nodeIterator_(
                const DisparityNode& node) {
            return this->nodes_.begin()
                + node.row * this->graph_.columns() + node.column;
        }
    public:
        Labeling() = delete;
        /**
         * \brief Copy labeling in default way.
         */
        Labeling(const Labeling&) = default;
        /**
         * \brief Move labeling in default way.
         */
        Labeling(Labeling&&) = default;
        /**
         * \brief Initialize the labeling based on disparities graph.
         */
        explicit Labeling(const DisparityGraph<Color>& graph)
                : graph_{graph}
                , nodes_{graph.availableNodes()} {
        }
        /**
         * \brief Just destroy the labeling.
         */
        ~Labeling() = default;
        /**
         * \brief Get available disparities for the node.
         */
        vector<size_t> nodeDisparities(const DisparityNode& node) {
            vector<size_t> result;

            multiset<size_t> disparities;
            vector<DisparityNode> neighbors = this->neighbors(node);
            for (DisparityNode neighbor : neighbors) {
                vector<size_t> neighborDisparities
                    = this->graph_.neighborDisparities(neighbor, node);
                disparities.insert(neighborDisparities.begin(),
                                   neighborDisparities.end());
            }
            if (disparities.empty()) {
                return result;
            }
            multiset<size_t>::iterator it = disparities.begin();
            while (it != disparities.end()) {
                size_t count = disparities.count(*it);
                assert(count <= neighbors.size());
                if (count == neighbors.size()) {
                    result.push_back(*it);
                }
                advance(it, count);
            }
            return result;
        }
        /**
         * \brief Calculate total penalty.
         */
        double penalty() {
            if (isfinite(this->penalty_)) {
                return this->penalty_;
            }
            this->penalty_ = 0;
            for (DisparityNode node : this->nodes_) {
                for (DisparityNode neighbor : this->neighbors(node, true)) {
                    this->penalty_ += this->graph_.penalty(node, neighbor);
                }
            }
            assert(isfinite(this->penalty_));
            return this->penalty_;
        }
        /**
         * \brief Change disparity of the node.
         */
        void setNode(const DisparityNode& node) {
            this->graph_.checkNode(node);
            vector<size_t> disparities = this->nodeDisparities(node);
            if (find(disparities.begin(), disparities.end(), node.disparity)
                    == disparities.end()) {
                throw invalid_argument("Provided disparity is not available.");
            }
            *(this->nodeIterator_(node)) = node;
            this->penalty_ = numeric_limits<double>::infinity();
        }
        /**
         * \brief Get disparity of the node.
         */
        size_t disparity(const DisparityNode& node) const {
            return (*this->nodeIterator_(node)).disparity;
        }
        /**
         * \brief Get neighbor nodes of the given one.
         */
        vector<DisparityNode> neighbors(
                const DisparityNode& node, bool directed = false) const {
            vector<DisparityNode> neighbors = this->graph_.nodeNeighbors(
                node, directed);
            vector<DisparityNode> result;
            result.reserve(neighbors.size());

            for (DisparityNode neighbor : neighbors) {
                vector<DisparityNode>::const_iterator it =
                    this->nodeIterator_(neighbor);
                assert((*it).row == neighbor.row);
                assert((*it).column == neighbor.column);
                result.push_back(*it);
            }
            return result;
        }
        /**
         * \brief Getter for constant references to nodes.
         */
        const vector<DisparityNode>& nodes() const {
            return this->nodes_;
        }
        /**
         * \brief Copy assignment operator.
         *
         * Needs to be implemented explicitly
         * because of const reference to DisparityGraph.
         *
         * We cannot change the graph,
         * so let it be an exception
         * in the case when graph of another labeling
         * is not the same.
         */
        Labeling& operator=(const Labeling& labeling) {
            if (&(this->graph_) != &(labeling.graph_)) {
                throw invalid_argument(
                    "You can assign only the labeling "
                    "with the same disparity graph.");
            }
            this->nodes_ = labeling.nodes_;
            this->penalty_ = labeling.penalty_;
            return *this;
        }
};

#endif
