#include "testdiffusion_disparity_finder.hpp"

#include "diffusion_disparity_finder.hpp"
#include "disparity_graph.hpp"
#include "labeling.hpp"
#include "matrix.hpp"

TEST(DiffusionDisparityFinderTest, CreateSuccessful)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    DisparityGraph<unsigned char> graph{left, right};
    DiffusionDisparityFinder<unsigned char> finder{graph};
}

TEST(DiffusionDisparityFinderTest, FindBestTrivial)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    DisparityGraph<unsigned char> graph{left, right};
    DiffusionDisparityFinder<unsigned char> finder{graph};
    Labeling<unsigned char> labeling{finder.find()};
    ASSERT_DOUBLE_EQ(labeling.penalty(), 0);
    for (DisparityNode node : labeling.nodes())
    {
        ASSERT_EQ(node.disparity, 0ul);
    }
}

TEST(DiffusionDisparityFinderTest, FindBestDot)
{
    Matrix<unsigned char> left{3, 3}, right{3, 3};
    left[1][1] = 0xFF;
    right[1][0] = 0xFF;
    DisparityGraph<unsigned char> graph{left, right};
    DiffusionDisparityFinder<unsigned char> finder{graph};
    Labeling<unsigned char> labeling{finder.find()};
    ASSERT_DOUBLE_EQ(labeling.penalty(), 3);
    ASSERT_EQ(labeling.disparity({1, 0}), 1ul);
}

TEST(DiffusionDisparityFinderTest, FindBest5x5)
{
    Matrix<unsigned char> left{5, 6}, right{5, 5};

    left[0] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};
    left[1] =  {0x00, 0x80, 0x80, 0x80, 0x00, 0xFF};
    left[2] =  {0x00, 0x80, 0xFF, 0x80, 0x00, 0xFF};
    left[3] =  {0x00, 0x80, 0x80, 0x80, 0x00, 0xFF};
    left[4] =  {0x00, 0x00, 0x00, 0x00, 0x00, 0xFF};

    right[0] = {0x00, 0x00, 0x00, 0x00, 0xFF};
    right[1] = {0x00, 0x80, 0x80, 0x00, 0xFF};
    right[2] = {0x00, 0xFF, 0x80, 0x00, 0xFF};
    right[3] = {0x00, 0x80, 0x80, 0x00, 0xFF};
    right[4] = {0x00, 0x00, 0x00, 0x00, 0xFF};

    DisparityGraph<unsigned char> graph{left, right};
    DiffusionDisparityFinder<unsigned char> finder{graph};
    Labeling<unsigned char> labeling{finder.find()};
    ASSERT_DOUBLE_EQ(labeling.penalty(), 5);
    ASSERT_EQ(labeling.disparity({2, 1}), 1ul);
}
