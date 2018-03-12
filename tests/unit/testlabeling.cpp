#include <vector>

#include "testlabeling.hpp"

#include "disparity_graph.hpp"
#include "matrix.hpp"
#include "labeling.hpp"

using std::vector;

TEST(LabelingTest, CreateSuccessful) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};
}

TEST(LabelingTest, RightAvailableDisparities) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    vector<size_t> disparities;
    
    disparities = labeling.nodeDisparities({9, 9});
    ASSERT_EQ(disparities.size(), 1);
    ASSERT_EQ(disparities.front(), 0);

    disparities = labeling.nodeDisparities({0, 0});
    ASSERT_EQ(disparities.size(), 2);
    ASSERT_EQ(disparities[0], 0);
    ASSERT_EQ(disparities[1], 1);
}

TEST(LabelingTest, SetNodeSuccess) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    ASSERT_EQ(labeling.disparity({0, 0}), 0);
    labeling.setNode({0, 0, 1});
    ASSERT_EQ(labeling.disparity({0, 0}), 1);
    labeling.setNode({0, 0, 0});
    ASSERT_EQ(labeling.disparity({0, 0}), 0);
}

TEST(LabelingTest, PenaltyRight) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    Labeling<unsigned char> labeling{graph};

    ASSERT_DOUBLE_EQ(labeling.penalty(), 0);
    labeling.setNode({0, 0, 1});
    ASSERT_DOUBLE_EQ(labeling.penalty(), 2);
}
