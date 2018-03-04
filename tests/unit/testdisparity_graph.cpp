#include "testdisparity_graph.hpp"
#include "matrix.hpp"

TEST(DisparityGraphTest, CreateSuccessful) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
}

TEST(DisparityGraphTest, NodesPenalty) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    right[0][0] = 3;
    left[0][0] = 1;
    left[0][1] = 2;
    DisparityGraph<unsigned char> graph{left, right};
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 0, 0}), 4);
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 0, 1}), 1);
}

TEST(DisparityGraphTest, EdgesPenalty) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    right[0][0] = 9;
    left[0][1] = 4;
    DisparityGraph<unsigned char> graph{left, right};
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 0, 1), 25);
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 1, 0), 16);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 0, 1}, {0, 1, 0}),
                     25. / 2 +  16. / 3 + 1);
}
