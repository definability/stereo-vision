#include <vector>

#include "testlabeling.hpp"

#include "disparity_graph.hpp"
#include "labeling.hpp"
#include "matrix.hpp"

using std::vector;

TEST(LabelingTest, CreateSuccessful)
{
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};
}

TEST(LabelingTest, RightAvailableDisparities)
{
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    vector<size_t> disparities;

    disparities = labeling.nodeDisparities({9, 9});
    ASSERT_EQ(disparities.size(), 1ul);
    ASSERT_EQ(disparities.front(), 0ul);

    disparities = labeling.nodeDisparities({0, 0});
    ASSERT_EQ(disparities.size(), 2ul);
    ASSERT_EQ(disparities[0], 0);
    ASSERT_EQ(disparities[1], 1);
}

TEST(LabelingTest, SetNodeSuccess)
{
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    ASSERT_EQ(labeling.disparity({0, 0}), 0);
    labeling.setNode({0, 0, 1});
    ASSERT_EQ(labeling.disparity({0, 0}), 1);
    labeling.setNode({0, 0, 0});
    ASSERT_EQ(labeling.disparity({0, 0}), 0);
}

TEST(LabelingTest, SetNodesSuccess)
{
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    ASSERT_EQ(labeling.disparity({5, 5}), 0);
    labeling.setNode({5, 5, 1});
    ASSERT_EQ(labeling.disparity({5, 5}), 1);
    labeling.setNode({5, 4, 2});
    ASSERT_EQ(labeling.disparity({5, 4}), 2);
    labeling.setNode({5, 3, 3});
    ASSERT_EQ(labeling.disparity({5, 3}), 3);
}

TEST(LabelingTest, PenaltyRight)
{
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    ASSERT_DOUBLE_EQ(labeling.penalty(), 0);
    labeling.setNode({0, 0, 1});
    ASSERT_DOUBLE_EQ(labeling.penalty(), 2);
    labeling.setNode({5, 5, 1});
    ASSERT_DOUBLE_EQ(labeling.penalty(), 2 + 4 * 1);
    labeling.setNode({5, 4, 2});
    ASSERT_DOUBLE_EQ(labeling.penalty(), 2 + 4 * 1 + 3 * 4);
    labeling.setNode({5, 3, 3});
    ASSERT_DOUBLE_EQ(labeling.penalty(), 2 + 4 * 1 + 2 * 4 + 1 * 1
                                       + 3 * 9);
}
