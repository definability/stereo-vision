#ifndef DIFFUSION_DISPARITY_FINDER
#define DIFFUSION_DISPARITY_FINDER

#include <cmath>
#include <limits>
#include <set>
#include <thread>
#include <utility>
#include <vector>
#include <iostream>

#include "disparity_finder.hpp"
#include "disparity_graph.hpp"
#include "labeling.hpp"
#include "boolean_graph.hpp"

using std::isfinite;
using std::move;
using std::numeric_limits;
using std::pair;
using std::set;
using std::thread;
using std::vector;
using std::cout;
using std::endl;

/**
 * \brief An entity that finds disparities using diffusion algorithm.
 */
template <typename Color> class DiffusionDisparityFinder
    : public DisparityFinder<Color>
{
    private:
        /**
         * \brief Variable to store current labeling.
         */
        Labeling<Color> labeling_;
        /**
         * \brief Boolean graph to check whether the fitting is finished.
         */
        BooleanGraph<Color> booleanGraph_;
        /**
         * \brief Additional penalty for edges.
         *
         * As an energy passing algorithm,
         * this one needs to store additional penalties of edges.
         */
        vector< vector< vector<double> > > passedPenalties_;
        /**
         * \brief Find the best labeling of final trivial problem.
         */
        Labeling<Color> getBestLabeling_()
        {
            set<DisparityNode> used;
            for (DisparityNode node: this->labeling_.nodes())
            {
                DisparityNode index = {node.row, node.column};
                if (used.find(index) != used.end())
                {
                    continue;
                }
                for (size_t disparity : this->graph_.nodeDisparities(node))
                {
                    node.disparity = disparity;
                    if (this->booleanGraph_.nodeAvailable(node))
                    {
                        this->labeling_.setNodeForce(node);
                        used.insert(index);
                        break;
                    }
                }
                assert(used.find(index) != used.end());
            }
            return this->labeling_;
        }
        /**
         * \brief An iteration of diffusion algorithm.
         *
         * The iteration performs following things for each node:
         *  - find the best edges from the node to each neighbor
         *  - decrease penalty of all neighbors
         *  by the average value of the best weights
         */
        void iteration_()
        {
            vector<thread> threads;
            size_t modulo = 1;
            for (size_t i = 0; i < modulo; ++i)
            {
                thread t(&DiffusionDisparityFinder::iterationThread_, this, modulo, i, true);
                threads.push_back(move(t));
            }
            for (thread& t : threads)
            {
                t.join();
            }
            threads.clear();

            for (size_t i = 0; i < modulo; ++i)
            {
                thread t(&DiffusionDisparityFinder::iterationThread_, this, modulo, i, false);
                threads.push_back(move(t));
            }
            for (thread& t : threads)
            {
                t.join();
            }
            threads.clear();
        }
        /**
         * \brief Helping function for multithreading.
         */
        void iterationThread_(size_t modulo, size_t offset, bool first)
        {
            for (DisparityNode node: this->labeling_.nodes())
            {
                if (first && ((node.row ^ node.column) & 1))
                {
                    continue;
                }
                else if (!first && !((node.row ^ node.column) & 1))
                {
                    continue;
                }
                else if (node.index % modulo != offset)
                {
                    continue;
                }
                for (size_t disparity : this->graph_.nodeDisparities(node))
                {
                    node.disparity = disparity;
                    this->processNode_(node);
                }
            }
        }
        /**
         * \brief Get additional penalty passed to edge
         * between specified node and neighbor.
         *
         * The function is symmetric,
         * so memory consumption was decreased twice.
         */
        double passedPenalty_(
            const DisparityNode& node,
            const DisparityNode& neighbor)
        {
            size_t neighborIndex = -1ul;
            size_t nodeNeighborIndex = -1ul;

            if (neighbor.row <= node.row && neighbor.column <= node.column)
            {
                neighborIndex =
                    2 * (node.row - neighbor.row)
                    + node.column - neighbor.column - 1;
                nodeNeighborIndex =
                    2 * (node.row - neighbor.row)
                    + node.column - neighbor.column + 1;
            }
            else if (neighbor.row >= node.row
                  && neighbor.column >= node.column)
            {
                neighborIndex =
                    2 * (neighbor.row - node.row)
                    + neighbor.column - node.column + 1;
                nodeNeighborIndex =
                    2 * (neighbor.row - node.row)
                    + neighbor.column - node.column - 1;
            }
            else
            {
                assert(false);
            }

            assert(this->passedPenalties_.size() > node.index);
            assert(
                this->passedPenalties_[node.index].size()
                > node.disparity);
            assert(
                this->passedPenalties_
                [node.index][node.disparity].size() > neighborIndex);

            return this->passedPenalties_
                    [node.index][node.disparity]
                    [neighborIndex]
                + this->passedPenalties_
                    [neighbor.index][neighbor.disparity]
                    [nodeNeighborIndex];
        }
        /**
         * \brief Change value of penalty passed to edge,
         * that connects specified node and neighbor.
         */
        void changePassedPenalty_(
            const DisparityNode& node,
            const DisparityNode& neighbor,
            double change)
        {
            size_t neighborIndex = -1ul;

            if (neighbor.row <= node.row && neighbor.column <= node.column)
            {
                neighborIndex =
                    2 * (node.row - neighbor.row)
                    + node.column - neighbor.column - 1;
            }
            else if (neighbor.row >= node.row
                  && neighbor.column >= node.column)
            {
                neighborIndex =
                    2 * (neighbor.row - node.row)
                    + neighbor.column - node.column + 1;
            }
            else
            {
                assert(false);
            }

            assert(this->passedPenalties_.size() > node.index);
            assert(this->passedPenalties_[node.index].size() > node.disparity);
            assert(
                this->passedPenalties_
                [node.index][node.disparity].size() > neighborIndex);

            this->passedPenalties_
                [node.index][node.disparity]
                [neighborIndex] += change;
        }
        /**
         * \brief Diffusion step for one node.
         *
         * The step consists of two substeps
         *  - find the best edges from the node to each neighbor
         *  - decrease penalty of all edges connecting node with its neighbors
         *  by the average value of the best weights
         */
        void processNode_(const DisparityNode& node)
        {
            double nodePenalty = 0;

            const vector<DisparityNode>& neighbors =
                this->graph_.nodeNeighbors(node);
            for (const DisparityNode& neighbor : neighbors)
            {
                double accumulatedPenalty = 0;
                accumulatedPenalty = this->minEdgePenalty_(
                    node,
                    neighbor
                );

                nodePenalty +=
                    accumulatedPenalty / neighbors.size();

                this->changePassedPenalty_(
                    node, neighbor,
                    -accumulatedPenalty);
            }

            for (const DisparityNode& neighbor : neighbors)
            {
                this->changePassedPenalty_(node, neighbor, nodePenalty);
            }
        }
        /**
         * \brief Calculate minimal penalty of edges
         * between specified node and neighbor.
         */
        double minEdgePenalty_(
            const DisparityNode& node,
            DisparityNode neighbor)
        {
            double minPenalty = numeric_limits<double>::infinity();
            size_t minDisparity = this->graph_.minNeighborDisparity(
                node, neighbor);
            size_t maxDisparity = this->graph_.maxNeighborDisparity(
                node, neighbor);
            for (size_t disparity = minDisparity;
                 disparity < maxDisparity;
                 ++disparity)
            {
                neighbor.disparity = disparity;
                double penalty = this->passedPenalty_(node, neighbor)
                    + this->graph_.penalty(node, neighbor);
                assert(isfinite(penalty));
                if (penalty < minPenalty) {
                    minPenalty = penalty;
                }
            }
            assert(isfinite(minPenalty));
            return minPenalty;
        }
        /**
         * \brief Check whether the problem became trivial
         * according to specified threshold.
         */
        bool isFinished_(double threshold)
        {
            this->initialiseAvailability_(threshold);
            bool result = this->booleanGraph_.isFinished();
            return result;
        }
        /**
         * \brief Initialize boolean availability graph.
         */
        void initialiseAvailability_(double threshold)
        {
            this->booleanGraph_.initialize();
            for (DisparityNode node: this->labeling_.nodes())
            {
                for (DisparityNode neighbor
                   : this->graph_.nodeNeighbors(node, true))
                {
                    double minPenalty = numeric_limits<double>::infinity();
                    for (size_t disparity : this->graph_.nodeDisparities(node))
                    {
                        node.disparity = disparity;
                        double currentMinPenalty =
                            this->minEdgePenalty_(node, neighbor);
                        if (minPenalty > currentMinPenalty)
                        {
                            minPenalty = currentMinPenalty;
                        }
                    }
                    minPenalty += threshold;
                    for (size_t disparity : this->graph_.nodeDisparities(node))
                    {
                        node.disparity = disparity;
                        size_t minDisparity = this->graph_.minNeighborDisparity(
                            node, neighbor);
                        size_t maxDisparity = this->graph_.maxNeighborDisparity(
                            node, neighbor);
                        for (size_t neighborDisparity = minDisparity;
                             neighborDisparity < maxDisparity;
                             ++neighborDisparity)
                        {
                            neighbor.disparity = neighborDisparity;
                            if (this->passedPenalty_(node, neighbor)
                                + this->graph_.penalty(node, neighbor)
                                > minPenalty)
                            {
                                this->booleanGraph_.removeEdge(node, neighbor);
                            }
                        }
                    }
                }
            }
        }
        /**
         * \brief Initialize data regarding passed penalties.
         */
        void initialize_()
        {
            const size_t MAX_NEIGHBORS_COUNT = 4;
            this->passedPenalties_.resize(this->labeling_.nodes().size());

            for (DisparityNode node: this->labeling_.nodes())
            {

                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;

                vector<size_t> disparities =
                    this->graph_.nodeDisparities(node);

                this->passedPenalties_[nodeIndex].resize(
                    *max_element(disparities.begin(), disparities.end())
                    + 1);
                for (size_t disparity : disparities)
                {
                    node.disparity = disparity;
                    this->passedPenalties_[nodeIndex][disparity].resize(
                        MAX_NEIGHBORS_COUNT);
                }
            }
        }
        /**
         * \brief Make all passed penalties zero.
         */
        void resetPenalties_()
        {
            for (DisparityNode node: this->labeling_.nodes())
            {
                size_t nodeIndex =
                    node.row * this->graph_.columns() + node.column;
                for (size_t disparity : this->graph_.nodeDisparities(node))
                {
                    node.disparity = disparity;
                    assert(
                        this->passedPenalties_[nodeIndex].size()
                        > disparity);
                    for (size_t neighborIndex = 0;
                         neighborIndex < this->passedPenalties_
                            [nodeIndex][disparity].size();
                         ++neighborIndex)
                    {
                        assert(
                            this->passedPenalties_
                                [nodeIndex][disparity].size()
                            > neighborIndex);
                        this->passedPenalties_
                            [nodeIndex][disparity]
                            [neighborIndex] = 0;
                    }
                }
            }
        }
    public:
        /**
         * \brief Forbid the default constructor.
         */
        DiffusionDisparityFinder() = delete;
        /**
         * \brief Copy constructor is default.
         */
        DiffusionDisparityFinder(const DiffusionDisparityFinder&) = default;
        /**
         * \brief Move constructor is default.
         */
        DiffusionDisparityFinder(DiffusionDisparityFinder&&) = default;
        /**
         * \brief Only a graph that describes the problem is needed.
         */
        explicit DiffusionDisparityFinder(const DisparityGraph<Color>& graph)
            : DisparityFinder<Color>{graph}
            , labeling_{graph}
            , booleanGraph_{graph}
        {
            this->initialize_();
        }
        /**
         * \brief Destructor is default.
         */
        virtual ~DiffusionDisparityFinder() = default;
        /**
         * \brief Find the best labeling using diffusion algorithm.
         */
        virtual Labeling<Color> find()
        {
            this->resetPenalties_();
            size_t i = 0;
            double threshold = 1
                / (2 * this->graph_.columns() * this->graph_.rows() * 4);
            while (!this->isFinished_(threshold))
            {
                cout << "Deletion " << ++i << endl;
                this->iteration_();
                cout << "Iteration " << i << endl;
            }
            cout << "Finished" << endl;
            return this->getBestLabeling_();
        }
};

#endif
