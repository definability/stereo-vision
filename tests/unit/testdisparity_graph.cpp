#include <limits>

#include "testdisparity_graph.hpp"
#include "matrix.hpp"

using std::numeric_limits;

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
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 1, 0}), 4);
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 0, 1}), 1);
}

TEST(DisparityGraphTest, EdgesPenalty) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    right[0][0] = 9;
    right[0][1] = 8;
    left[0][0] = 4;
    left[0][2] = 5;
    DisparityGraph<unsigned char> graph{left, right};
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 0, 0), 25);
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 1, 1), 9);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 0, 0}, {0, 1, 1}),
                     25. / 2 +  9. / 3 + 1);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 1, 1}, {0, 0, 0}),
                     25. / 2 +  9. / 3 + 1);

    ASSERT_DOUBLE_EQ(graph.penalty({0, 5, 2}, {0, 6, 3}), 1);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 6, 3}, {0, 5, 2}), 1);

    ASSERT_TRUE(graph.edgeExists({0, 5, 2}, {0, 6, 3}));
    ASSERT_TRUE(graph.edgeExists({0, 6, 3}, {0, 5, 2}));
}

TEST(DisparityGraphTest, DiagonalInf) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};

    ASSERT_DOUBLE_EQ(graph.penalty({1, 0}, {0, 1}),
                     numeric_limits<double>::infinity());
    ASSERT_DOUBLE_EQ(graph.penalty({0, 1}, {1, 0}),
                     numeric_limits<double>::infinity());

    ASSERT_FALSE(graph.edgeExists({1, 0}, {0, 1}));
    ASSERT_FALSE(graph.edgeExists({0, 1}, {1, 0}));
}

TEST(DisparityGraphTest, FarHorizontalInf) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};

    ASSERT_DOUBLE_EQ(graph.penalty({0, 0}, {0, 5}),
                     numeric_limits<double>::infinity());
    ASSERT_DOUBLE_EQ(graph.penalty({0, 5}, {0, 0}),
                     numeric_limits<double>::infinity());

    ASSERT_FALSE(graph.edgeExists({0, 0}, {0, 5}));
    ASSERT_FALSE(graph.edgeExists({0, 5}, {0, 0}));
}

TEST(DisparityGraphTest, FarVerticalInf) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};

    ASSERT_DOUBLE_EQ(graph.penalty({0, 0}, {5, 0}),
                     numeric_limits<double>::infinity());
    ASSERT_DOUBLE_EQ(graph.penalty({5, 0}, {0, 0}),
                     numeric_limits<double>::infinity());

    ASSERT_FALSE(graph.edgeExists({0, 0}, {5, 0}));
    ASSERT_FALSE(graph.edgeExists({5, 0}, {0, 0}));
}

TEST(DisparityGraphTest, WrongDisparitiesInf) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};

    ASSERT_DOUBLE_EQ(graph.penalty({0, 5, 3}, {0, 6, 1}),
                     numeric_limits<double>::infinity());
    ASSERT_DOUBLE_EQ(graph.penalty({0, 6, 1}, {0, 5, 3}),
                     numeric_limits<double>::infinity());

    ASSERT_FALSE(graph.edgeExists({0, 5, 3}, {0, 6, 1}));
    ASSERT_FALSE(graph.edgeExists({0, 6, 1}, {0, 5, 3}));
}
