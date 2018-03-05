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
    right[0][1] = 3;
    left[0][0] = 1;
    left[0][1] = 2;
    DisparityGraph<unsigned char> graph{left, right};
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 1, 0}), 1);
    ASSERT_DOUBLE_EQ(graph.nodePenalty({0, 1, 1}), 4);
}

TEST(DisparityGraphTest, EdgesPenalty) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    right[0][0] = 9;
    left[0][0] = 4;
    DisparityGraph<unsigned char> graph{left, right};
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 0, 0), 25);
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 1, 1), 16);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 0, 0}, {0, 1, 1}),
                     25. / 2 +  16. / 3 + 1);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 1, 1}, {0, 0, 0}),
                     25. / 2 +  16. / 3 + 1);

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

    ASSERT_DOUBLE_EQ(graph.penalty({0, 5, 2}, {0, 6, 4}),
                     numeric_limits<double>::infinity());
    ASSERT_DOUBLE_EQ(graph.penalty({0, 6, 4}, {0, 5, 2}),
                     numeric_limits<double>::infinity());

    ASSERT_FALSE(graph.edgeExists({0, 5, 2}, {0, 6, 4}));
    ASSERT_FALSE(graph.edgeExists({0, 6, 4}, {0, 5, 2}));
}
