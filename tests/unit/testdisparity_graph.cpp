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

TEST(DisparityGraphTest, ConsistencyAffectsWeight) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    right[0][0] = 9;
    right[0][1] = 8;
    left[0][0] = 4;
    left[0][2] = 5;
    DisparityGraph<unsigned char> graph{left, right, 10};
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 0, 0), 25);
    ASSERT_DOUBLE_EQ(graph.nodePenalty(0, 1, 1), 9);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 0, 0}, {0, 1, 1}),
                     25. / 2 +  9. / 3 + 10 * 1);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 1, 1}, {0, 0, 0}),
                     25. / 2 +  9. / 3 + 10 * 1);

    ASSERT_DOUBLE_EQ(graph.penalty({0, 5, 2}, {0, 6, 3}), 10);
    ASSERT_DOUBLE_EQ(graph.penalty({0, 6, 3}, {0, 5, 2}), 10);

    ASSERT_TRUE(graph.edgeExists({0, 5, 2}, {0, 6, 3}));
    ASSERT_TRUE(graph.edgeExists({0, 6, 3}, {0, 5, 2}));
}

TEST(DisparityGraphTest, GetAllNodes) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph graph{left, right};
    for (auto item: graph.availableNodes()) {
        ASSERT_EQ(item.disparity, 0);
    }
    ASSERT_EQ(graph.availableNodes().size(), 100);
}

TEST(DisparityGraphTest, GetNodeNeighbors) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph graph{left, right};

    vector<DisparityNode> neighbors;

    neighbors = graph.nodeNeighbors({0, 0}, false);

    ASSERT_EQ(neighbors.size(), 2);
    ASSERT_EQ(neighbors[0].row, 0);
    ASSERT_EQ(neighbors[0].column, 1);
    ASSERT_EQ(neighbors[1].row, 1);
    ASSERT_EQ(neighbors[1].column, 0);
    for (auto neighbor : neighbors) {
        ASSERT_TRUE(graph.edgeExists({0, 0}, neighbor));
    }

    neighbors = graph.nodeNeighbors({5, 6}, true);

    ASSERT_EQ(neighbors.size(), 2);
    ASSERT_EQ(neighbors[0].row, 5);
    ASSERT_EQ(neighbors[0].column, 7);
    ASSERT_EQ(neighbors[1].row, 6);
    ASSERT_EQ(neighbors[1].column, 6);
    for (auto neighbor : neighbors) {
        ASSERT_TRUE(graph.edgeExists({5, 6}, neighbor));
    }

    ASSERT_EQ(graph.nodeNeighbors({9, 9}, true).size(), 0);
}
