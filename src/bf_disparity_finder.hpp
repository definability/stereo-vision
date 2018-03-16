#ifndef BFDISPARITY_FINDER
#define BFDISPARITY_FINDER

#include <limits>
#include <vector>

#include "disparity_graph.hpp"
#include "labeling.hpp"

using std::numeric_limits;
using std::vector;

/**
 * \brief An entity that finds disparities using brute force.
 */
template <typename Color> class BFDisparityFinder {
    private:
        /**
         * \brief A graph that represents the problem to solve.
         */
        const DisparityGraph<Color>& graph_;
        /**
         * \brief Find the best labeling.
         *
         * For every disparity of current node
         * check whether this disparity gives the best penalty
         * and call the procedure for the next node if it exists.
         */
        Labeling<Color> find_(
                Labeling<Color>& labeling,
                Labeling<Color>& bestLabeling,
                vector<DisparityNode>::const_iterator currentNode) {
            if (currentNode == labeling.nodes().end()) {
                return bestLabeling;
            }

            for (size_t disparity : labeling.nodeDisparities(*currentNode)) {
                labeling.setNode({
                    currentNode->row,
                    currentNode->column,
                    disparity
                });

                if (labeling.penalty() < bestLabeling.penalty()) {
                    bestLabeling = labeling;
                }

                bestLabeling = this->find_(labeling, bestLabeling, next(currentNode));
            }
            return bestLabeling;
        }
    public:
        /**
         * \brief Forbid the default constructor.
         */
        BFDisparityFinder() = delete;
        /**
         * \brief Copy constructor is default.
         */
        BFDisparityFinder(const BFDisparityFinder&) = default;
        /**
         * \brief Move constructor is default.
         */
        BFDisparityFinder(BFDisparityFinder&&) = default;
        /**
         * \brief Only a graph that describes the problem is needed.
         */
        explicit BFDisparityFinder(const DisparityGraph<Color>& graph)
            : graph_{graph} {
        }
        /**
         * \brief Find the best labeling using brute force.
         */
        Labeling<Color> find() {
            Labeling<Color> labeling{this->graph_};
            Labeling<Color> bestLabeling{labeling};
            return this->find_(
                labeling,
                bestLabeling,
                labeling.nodes().begin()
            );
        }
};

#endif
