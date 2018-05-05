#include "testbf_disparity_finder.hpp"

#include "matrix.hpp"
#include "labeling.hpp"
#include "disparity_graph.hpp"
#include "bf_disparity_finder.hpp"

TEST(BFDisparityFinderTest, CreateSuccessful)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    DisparityGraph<unsigned char> graph{left, right};
    BFDisparityFinder<unsigned char> finder{graph};
}

TEST(BFDisparityFinderTest, FindBestTrivial)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    DisparityGraph<unsigned char> graph{left, right};
    BFDisparityFinder<unsigned char> finder{graph};
    Labeling<unsigned char> labeling{finder.find()};
    ASSERT_DOUBLE_EQ(labeling.penalty(), 0);
    for (DisparityNode node : labeling.nodes())
    {
        ASSERT_EQ(node.disparity, 0ul);
    }
}

TEST(BFDisparityFinderTest, FindBestDot)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    left[1][1] = 0xFF;
    right[1][0] = 0xFF;
    DisparityGraph<unsigned char> graph{left, right};
    BFDisparityFinder<unsigned char> finder{graph};
    Labeling<unsigned char> labeling{finder.find()};
    ASSERT_DOUBLE_EQ(labeling.penalty(), 3);
    ASSERT_EQ(labeling.disparity({1, 0}), 1ul);
}
