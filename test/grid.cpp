#include <gtest/gtest.h>
#include "../src/solutions/grid.h"

TEST(Grid, add_row) {
    auto grid = make_grid("..\n..\n");

    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);

    /*
     * ..
     * ..
     * ##
     */
    grid.add_row("##");
    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 3);
    ASSERT_EQ(grid.at({0, 0}), '.');
    ASSERT_EQ(grid.at({0, 1}), '.');
    ASSERT_EQ(grid.at({1, 0}), '.');
    ASSERT_EQ(grid.at({1, 1}), '.');
    ASSERT_EQ(grid.at({2, 0}), '#');
    ASSERT_EQ(grid.at({2, 1}), '#');
}

TEST(Grid, add_row_start) {
    auto grid = make_grid("..\n..\n");

    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);

    /*
     * ##
     * ..
     * ..
     */
    grid.add_row_start("##");
    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 3);
    ASSERT_EQ(grid.at({0, 0}), '#');
    ASSERT_EQ(grid.at({0, 1}), '#');
    ASSERT_EQ(grid.at({1, 0}), '.');
    ASSERT_EQ(grid.at({1, 1}), '.');
    ASSERT_EQ(grid.at({2, 0}), '.');
    ASSERT_EQ(grid.at({2, 1}), '.');
}

TEST(grid, add_column) {
    auto grid = make_grid("..\n..");

    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);

    /*
     * ..#
     * ..#
     */
    grid.add_column("##");
    ASSERT_EQ(grid.columns, 3);
    ASSERT_EQ(grid.rows, 2);
    ASSERT_EQ(grid.at({0, 0}), '.');
    ASSERT_EQ(grid.at({0, 1}), '.');
    ASSERT_EQ(grid.at({0, 2}), '#');
    ASSERT_EQ(grid.at({1, 0}), '.');
    ASSERT_EQ(grid.at({1, 1}), '.');
    ASSERT_EQ(grid.at({1, 2}), '#');
}

TEST(grid, add_row_char) {
    auto grid = make_grid(".");
    ASSERT_EQ(grid.columns, 1);
    ASSERT_EQ(grid.rows, 1);

    grid.add_column('.');
    grid.add_column('.');

    for (int i = 0; i < 9; ++i) {
        grid.add_row('.');
    }

    ASSERT_EQ(grid.columns, 3);
    ASSERT_EQ(grid.rows, 10);
}

TEST(grid, add_column_char) {
    auto grid = make_grid(".");
    ASSERT_EQ(grid.columns, 1);
    ASSERT_EQ(grid.rows, 1);

    grid.add_row('.');
    grid.add_row('.');

    for (int i = 0; i < 9; ++i) {
        grid.add_column(std::to_string(i).at(0));
    }

    ASSERT_EQ(grid.columns, 10);
    ASSERT_EQ(grid.rows, 3);

    ASSERT_EQ(grid.at({0, 9}), '8');
    ASSERT_EQ(grid.at({1, 9}), '8');
    ASSERT_EQ(grid.at({2, 9}), '8');
}

TEST(grid, add_column_start) {
    auto grid = make_grid("..\n..\n");
    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);

    /*
     * #..
     * #..
     */
    grid.add_column_start("##");
    ASSERT_EQ(grid.columns, 3);
    ASSERT_EQ(grid.rows, 2);
    ASSERT_EQ(grid.at({0, 0}), '#');
    ASSERT_EQ(grid.at({0, 1}), '.');
    ASSERT_EQ(grid.at({0, 2}), '.');
    ASSERT_EQ(grid.at({1, 0}), '#');
    ASSERT_EQ(grid.at({1, 1}), '.');
    ASSERT_EQ(grid.at({1, 2}), '.');
}

TEST(grid, rotate_cw) {
    auto grid = make_grid("12\n34\n");

    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);
    ASSERT_EQ(grid.at({0, 0}), '1');
    ASSERT_EQ(grid.at({0, 1}), '2');
    ASSERT_EQ(grid.at({1, 0}), '3');
    ASSERT_EQ(grid.at({1, 1}), '4');

    /*
     * 12 => 31
     * 34 => 42
     */
    grid.rotate_cw();
    ASSERT_EQ(grid.columns, 2);
    ASSERT_EQ(grid.rows, 2);
    ASSERT_EQ(grid.at({0, 0}), '3');
    ASSERT_EQ(grid.at({0, 1}), '1');
    ASSERT_EQ(grid.at({1, 0}), '4');
    ASSERT_EQ(grid.at({1, 1}), '2');

    /*
     * 12 => 531
     * 34 => 642
     * 56 =>
     */
    auto grid_uneven = make_grid("12\n34\n56\n");

    ASSERT_EQ(grid_uneven.columns, 2);
    ASSERT_EQ(grid_uneven.rows, 3);
    ASSERT_EQ(grid_uneven.at({0, 0}), '1');
    ASSERT_EQ(grid_uneven.at({0, 1}), '2');
    ASSERT_EQ(grid_uneven.at({1, 0}), '3');
    ASSERT_EQ(grid_uneven.at({1, 1}), '4');
    ASSERT_EQ(grid_uneven.at({2, 0}), '5');
    ASSERT_EQ(grid_uneven.at({2, 1}), '6');

    grid_uneven.rotate_cw();
    ASSERT_EQ(grid_uneven.columns, 3);
    ASSERT_EQ(grid_uneven.rows, 2);
    ASSERT_EQ(grid_uneven.at({0, 0}), '5');
    ASSERT_EQ(grid_uneven.at({0, 1}), '3');
    ASSERT_EQ(grid_uneven.at({0, 2}), '1');
    ASSERT_EQ(grid_uneven.at({1, 0}), '6');
    ASSERT_EQ(grid_uneven.at({1, 1}), '4');
    ASSERT_EQ(grid_uneven.at({1, 2}), '2');
}