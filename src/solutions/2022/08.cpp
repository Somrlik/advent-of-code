#include "../one_solution.h"
#include "../grid.h"
#include <map>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
30373
25512
65332
33549
35390
)";

namespace {
    enum Direction {
        TOP,
        RIGHT,
        BOTTOM,
        LEFT,
    };
}

static std::map<Direction, GridCell> direction_to_vector = {
    {TOP, {-1, 0}},
    {RIGHT, {0, +1}},
    {BOTTOM, {+1, 0}},
    {LEFT, {0, -1}},
};

static long count_visible_from_directions(const Grid &grid) {
    std::set<GridCell> visible_trees;

    for (long row = 0; row < grid.rows; ++row) {
        long right_max = std::numeric_limits<long>::min();
        long left_max = std::numeric_limits<long>::min();

        for (long column = 0; column < grid.columns; ++column) {
            auto right_cell = GridCell{row, (long) (grid.columns - column - 1)};
            auto right_value = std::stol(std::string{grid.at(right_cell)});
            if (right_value > right_max) {
                visible_trees.insert(right_cell);
                right_max = right_value;
            }

            auto left_cell = GridCell{row, column};
            auto left_value = std::stol(std::string{grid.at(left_cell)});
            if (left_value > left_max) {
                visible_trees.insert(left_cell);
                left_max = left_value;
            }
        }
    }

    for (long column = 0; column < grid.columns; ++column) {
        long top_max = std::numeric_limits<long>::min();
        long bottom_max = std::numeric_limits<long>::min();

        for (long row = 0; row < grid.rows; ++row) {
            auto top_cell = GridCell{row, column};
            auto top_value = std::stol(std::string{grid.at(top_cell)});
            if (top_value > top_max) {
                visible_trees.insert(top_cell);
                top_max = top_value;
            }

            auto bottom_cell = GridCell{(long)(grid.rows - row - 1), column};
            auto bottom_value = std::stol(std::string{grid.at(bottom_cell)});
            if (bottom_value > bottom_max) {
                visible_trees.insert(bottom_cell);
                bottom_max = bottom_value;
            }
        }
    }

    return (long)visible_trees.size();
}

SOLVER(2022, 8, 1, true)
(const std::string &in) {
    auto grid = make_grid(in);
//    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    auto result = count_visible_from_directions(grid);

    return fmt::format("{}", result);
}

static long determine_highest_scenic_score(const Grid &grid) {
    long highest_score = std::numeric_limits<long>::min();

    // Ignore edges, since they have 0 value
    for (long row = 1; row < (grid.rows - 1); ++row) {
        for (long column = 1; column < (grid.columns - 1); ++column) {
            auto cell = GridCell{row, column};
            auto cell_value = std::stol(std::string{grid.at(cell)});
            long scenic_score = 1; // Will be at least 1

            for (Direction direction: std::array{TOP, RIGHT, BOTTOM, LEFT}) {
                long score_acc = 0;
                auto direction_vector = direction_to_vector.at(direction);

                for (int acc = 1;; ++acc) {
                    auto cell_char = grid.at(cell + (direction_vector * acc));
                    if (cell_char == '\0') {
                        break;
                    }

                    auto current_value = std::stol(std::string{cell_char});
                    if (current_value >= cell_value) {
                        score_acc++;
                        break;
                    }

                    score_acc++;
                }

                scenic_score *= score_acc;
            }

            highest_score = std::max(scenic_score, highest_score);
        }
    }

    return highest_score;
}

SOLVER(2022, 8, 2, true)
(const std::string &in) {
        auto grid = make_grid(in);
//    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    auto score = determine_highest_scenic_score(grid);

    return fmt::format("{}", score);
}
