#include "testmatrix.hpp"

TEST(MatrixTest, CreateSuccessful) {
    Matrix<int> m{10, 20};
    ASSERT_EQ(m.rows(), 10u);
    ASSERT_EQ(m.columns(), 20u);
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 20; ++j) {
            ASSERT_EQ(m[i][j], 0);
        }
    }
}

TEST(MatrixTest, AssignSuccessful) {
    Matrix<int> m{10, 20};
    m[0][10] = 100;
    for (size_t i = 0; i < 10; ++i) {
        for (size_t j = 0; j < 20; ++j) {
            if (i == 0 && j == 10) {
                ASSERT_EQ(m[i][j], 100);
            }
            else {
                ASSERT_EQ(m[i][j], 0);
            }
        }
    }
}
