#include <tuple>

#include "gtest/gtest.h"
#include "disparity_graph.hpp"

using std::tuple;

class DisparityGraphInfiniteEdgesTest
    : public ::testing::TestWithParam<
        tuple<DisparityNode, DisparityNode>
      >
{
    protected:
        DisparityGraphInfiniteEdgesTest() {};
        virtual ~DisparityGraphInfiniteEdgesTest() {};
        virtual void SetUp() {};
        virtual void TearDown() {};
};
