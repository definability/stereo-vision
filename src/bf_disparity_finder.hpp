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
        const DisparityGraph<Color>& graph_;
        Labeling<Color> labeling_;

        double bestPenalty_ = numeric_limits<double>::infinity();
        Labeling<Color> bestLabeling_;

        void change_(
                vector<DisparityNode>::const_iterator currentNode,
                vector<DisparityNode>::const_iterator end) {

            if (currentNode == end) {
                return;
            }

            for (size_t disparity
                    : this->labeling_.nodeDisparities(*currentNode)) {

                this->labeling_.setNode({
                    currentNode->row,
                    currentNode->column,
                    disparity
                });
                double penalty = this->labeling_.penalty();
                if (penalty < this->bestPenalty_) {
                    this->bestLabeling_ = this->labeling_;
                    this->bestPenalty_ = penalty;
                }

                this->change_(next(currentNode), end);
            }
        }
    public:
        BFDisparityFinder() = delete;
        BFDisparityFinder(const BFDisparityFinder&) = default;
        explicit BFDisparityFinder(const DisparityGraph<Color>& graph)
            : graph_{graph}
            , labeling_{graph}
            , bestLabeling_{graph} {
        }
        Labeling<Color> find() {
            vector<DisparityNode> nodes{this->labeling_.nodes()};
            this->change_(nodes.begin(), nodes.end());
            return this->bestLabeling_;
        }
};

#endif
