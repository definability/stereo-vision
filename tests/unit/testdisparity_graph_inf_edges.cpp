#include <limits>
#include <tuple>

#include "testdisparity_graph_inf_edges.hpp"
#include "matrix.hpp"

using std::numeric_limits;
using std::make_tuple;
using std::get;

static double infinity = numeric_limits<double>::infinity();

TEST_P(DisparityGraphInfiniteEdgesTest, CheckParameters) {
    Matrix<unsigned char> left{10, 10}, right{10, 10};
    DisparityGraph<unsigned char> graph{left, right};
    DisparityNode nodeA = get<0>(GetParam());
    DisparityNode nodeB = get<1>(GetParam());

    ASSERT_DOUBLE_EQ(graph.penalty(nodeA, nodeB), infinity);
    ASSERT_DOUBLE_EQ(graph.penalty(nodeB, nodeA), infinity);

    ASSERT_FALSE(graph.edgeExists(nodeA, nodeB));
    ASSERT_FALSE(graph.edgeExists(nodeB, nodeA));
}

INSTANTIATE_TEST_CASE_P(
    CheckInfiniteEdges, DisparityGraphInfiniteEdgesTest,
    ::testing::Values(
        make_tuple(DisparityNode{1, 0}, DisparityNode{0, 1}),
        make_tuple(DisparityNode{0, 0}, DisparityNode{0, 5}),
        make_tuple(DisparityNode{0, 0}, DisparityNode{5, 0}),
        make_tuple(DisparityNode{0, 5, 3}, DisparityNode{0, 6, 1})
    )
);
