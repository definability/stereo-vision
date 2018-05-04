#ifndef BOOLEAN_GRAPH_HPP
#define BOOLEAN_GRAPH_HPP

#include <algorithm>
#include <utility>
#include <vector>
#include <iostream>

using std::cout;
using std::endl;
using std::max_element;
using std::none_of;
using std::pair;
using std::vector;

template<typename Color> class BooleanGraph {
    private:
        const DisparityGraph<Color>& graph_;
        vector <DisparityNode> nodes_;
        vector< vector<bool> > nodesAvailability_;
        vector< vector< vector< vector<bool> > > > edgesAvailability_;

        bool edgeAvailable_(
            const DisparityNode& node,
            const DisparityNode& neighbor)
        {
            if (node < neighbor)
            {
                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;
                size_t neighborIndex =
                    2 * (neighbor.row - node.row)
                    + neighbor.column - node.column - 1;
                return this->edgesAvailability_
                    [nodeIndex][node.disparity]
                    [neighborIndex][neighbor.disparity];
            }
            else
            {
                return this->edgeAvailable_(neighbor, node);
            }
        }

        void removeNode_(const DisparityNode& node)
        {
            size_t nodeIndex = node.row * this->graph_.columns() + node.column;
            this->nodesAvailability_[nodeIndex][node.disparity] = false;
            for (DisparityNode neighbor : this->graph_.nodeNeighbors(node))
            {
                for (size_t neighborDisparity = this->graph_.minNeighborDisparity(node, neighbor); neighborDisparity < this->graph_.maxNeighborDisparity(node, neighbor); ++neighborDisparity)
                {
                    neighbor.disparity = neighborDisparity;
                    this->removeEdge(node, neighbor);
                    assert(!this->edgeAvailable_(neighbor, node));
                }
            }
        }

        bool deletionIteration_()
        {
            bool changed = false;

            bool graphExists = false;
            for (DisparityNode node: this->nodes_)
            {
                graphExists = false;
                for (size_t disparity = this->graph_.minDisparity(node); disparity < this->graph_.maxDisparity(node); ++disparity)
                {
                    node.disparity = disparity;
                    if (!this->nodeAvailable(node))
                    {
                        continue;
                    }
                    for (DisparityNode neighbor
                       : this->graph_.nodeNeighbors(node))
                    {
                        bool nodeAvailable = false;
                        for (size_t neighborDisparity = this->graph_.minNeighborDisparity(node, neighbor); neighborDisparity < this->graph_.maxNeighborDisparity(node, neighbor); ++neighborDisparity)
                        {
                            neighbor.disparity = neighborDisparity;
                            if (this->edgeAvailable_(node, neighbor))
                            {
                                nodeAvailable = true;
                                break;
                            }
                        }
                        if (!nodeAvailable)
                        {
                            changed = true;
                            this->removeNode_(node);
                        }
                        else
                        {
                            graphExists = true;
                        }
                    }
                }
            }

            if (!graphExists)
            {
                for (DisparityNode node: this->nodes_)
                {
                    for (size_t disparity = this->graph_.minDisparity(node); disparity < this->graph_.maxDisparity(node); ++disparity)
                    {
                        node.disparity = disparity;
                        this->removeNode_(node);
                    }
                }
                changed = false;
            }

            return changed;
        }

        void initialize_()
        {
            this->nodesAvailability_.clear();
            this->nodesAvailability_.resize(this->nodes_.size());
            this->edgesAvailability_.clear();
            this->edgesAvailability_.resize(this->nodes_.size());

            for (DisparityNode node: this->nodes_)
            {

                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;

                vector<size_t> disparities =
                    this->graph_.nodeDisparities(node);

                this->nodesAvailability_[nodeIndex].resize(
                    *max_element(disparities.begin(), disparities.end())
                    + 1);
                this->edgesAvailability_[nodeIndex].resize(
                    *max_element(disparities.begin(), disparities.end())
                    + 1);
                for (size_t disparity : disparities)
                {
                    node.disparity = disparity;
                    this->edgesAvailability_[nodeIndex][disparity].resize(
                        this->graph_.nodeNeighborsCount(node, false));
                    for (DisparityNode neighbor
                       : this->graph_.nodeNeighbors(node, true))
                    {
                        assert(
                            neighbor.row >= node.row
                            && neighbor.column >= node.column);
                        size_t neighborIndex =
                            2 * (neighbor.row - node.row)
                            + neighbor.column - node.column - 1;
                        assert(
                            this->edgesAvailability_
                                [nodeIndex][disparity].size()
                            > neighborIndex);
                        this->edgesAvailability_
                            [nodeIndex][disparity]
                            [neighborIndex].clear();

                        vector<size_t> neighborDisparities =
                            this->graph_.neighborDisparities(node, neighbor);
                        this->edgesAvailability_
                            [nodeIndex][disparity]
                            [neighborIndex].resize(
                                *max_element(
                                    neighborDisparities.begin(),
                                    neighborDisparities.end()
                                )
                                + 1
                            );
                    }
                }
            }
        }
    public:
        BooleanGraph() = delete;
        BooleanGraph(const DisparityGraph<Color>& graph)
            : graph_{graph}
            , nodes_{graph.availableNodes()}
        {
            this->initialize_();
        }
        BooleanGraph(const BooleanGraph&) = default;
        BooleanGraph(BooleanGraph&&) = default;
        ~BooleanGraph() = default;

        void initialize()
        {
            for (DisparityNode node: this->nodes_)
            {
                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;
                for (size_t disparity = this->graph_.minDisparity(node); disparity < this->graph_.maxDisparity(node); ++disparity)
                {
                    node.disparity = disparity;
                    assert(
                        this->edgesAvailability_[nodeIndex].size()
                        > disparity);
                    this->nodesAvailability_[nodeIndex][disparity] = true;
                    assert(this->nodeAvailable(node));
                    for (DisparityNode neighbor
                       : this->graph_.nodeNeighbors(node, true))
                    {
                        size_t neighborIndex =
                            2 * (neighbor.row - node.row)
                            + neighbor.column - node.column - 1;
                        assert(
                            this->edgesAvailability_
                                [nodeIndex][disparity].size()
                            > neighborIndex);
                        for (size_t neighborDisparity = this->graph_.minNeighborDisparity(node, neighbor); neighborDisparity < this->graph_.maxNeighborDisparity(node, neighbor); ++neighborDisparity)
                        {
                            neighbor.disparity = neighborDisparity;
                            this->edgesAvailability_
                                [nodeIndex][disparity]
                                [neighborIndex][neighborDisparity] = true;
                            assert(this->edgeAvailable_(node, neighbor));
                            assert(this->edgeAvailable_(neighbor, node));
                        }
                    }
                }
            }
        }
        bool isFinished()
        {
            while(this->deletionIteration_());

            for (DisparityNode node : this->nodes_)
            {
                for (size_t disparity = this->graph_.minDisparity(node); disparity < this->graph_.maxDisparity(node); ++disparity)
                {
                    node.disparity = disparity;
                    if (this->nodeAvailable(node))
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        void removeEdge(
            const DisparityNode& node,
            const DisparityNode& neighbor)
        {
            if (node < neighbor)
            {
                assert(node.row <= neighbor.row);
                assert(node.column <= neighbor.column);
                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;
                size_t neighborIndex =
                    2 * (neighbor.row - node.row)
                    + neighbor.column - node.column - 1;
                assert(this->edgesAvailability_.size() > nodeIndex);
                assert(
                    this->edgesAvailability_[nodeIndex].size()
                    > node.disparity);
                assert(
                    this->edgesAvailability_
                    [nodeIndex][node.disparity].size() > neighborIndex);
                assert(
                    this->edgesAvailability_
                    [nodeIndex][node.disparity]
                    [neighborIndex].size() > neighbor.disparity);
                this->edgesAvailability_
                    [nodeIndex][node.disparity]
                    [neighborIndex][neighbor.disparity] = false;
                assert(!edgeAvailable_(neighbor, node));
                assert(!edgeAvailable_(node, neighbor));
            }
            else
            {
                this->removeEdge(neighbor, node);
            }
        }

        bool nodeAvailable(const DisparityNode& node)
        {
            return this->nodesAvailability_
                [node.row * this->graph_.columns() + node.column]
                [node.disparity];
        }
};

#endif
