#ifndef DISPARITY_FINDER
#define DISPARITY_FINDER

#include "disparity_graph.hpp"
#include "labeling.hpp"

/**
 * \brief Abstract class for disparity finders.
 *
 * Describes the basic method find()
 * and contains a graph describing the problem to solve.
 */
template <typename Color> class DisparityFinder
{
    protected:
        /**
         * \brief A graph that represents the problem to solve.
         */
        const DisparityGraph<Color>& graph_;
    public:
        /**
         * \brief Forbid the default constructor.
         */
        DisparityFinder() = delete;
        /**
         * \brief Copy constructor is default.
         */
        DisparityFinder(const DisparityFinder&) = default;
        /**
         * \brief Move constructor is default.
         */
        DisparityFinder(DisparityFinder&&) = default;
        /**
         * \brief A graph that describes the problem
         * is needed for any strategy.
         */
        explicit DisparityFinder(const DisparityGraph<Color>& graph)
            : graph_{graph}
        {
        }
        /**
         * \brief Destructor is default.
         */
        virtual ~DisparityFinder() = default;
        /**
         * \brief Find the best labeling.
         */
        virtual Labeling<Color> find();
};

#endif
