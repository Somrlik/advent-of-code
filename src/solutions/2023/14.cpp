#include "../one_solution.h"
#include "../grid.h"
#include <map>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
O....#....
O.OO#....#
.....##...
OO.#O....O
.O.....O#.
O.#..O.#.#
..O..#O..O
.......O..
#....###..
#OO..#....
)";

namespace {
    typedef enum WalkDirection {
        NORTH = 0,
        EAST,
        SOUTH,
        WEST,
    } WalkDirection;
}

static const std::map<WalkDirection, int> direction_to_cw_rotations = {
        {NORTH, 0},
        {EAST,  1},
        {SOUTH, 2},
        {WEST,  3},
};

static Grid walk_boulders(const Grid& in_grid, const WalkDirection& direction) {
    auto grid = in_grid; // Copy the grid

    // 1. Rotate the grid to point north, since I am lazy to actually solve properly for other directions
    auto cw_rotations = direction_to_cw_rotations.at(direction);
    for (int rotation = 0; rotation < cw_rotations; rotation++) {
        grid.rotate_cw();
    }

    // 2. Find the boulders in order
    for (int row_idx = 0; row_idx < grid.rows; row_idx++) {
        for (int column_idx = 0; column_idx < grid.columns; column_idx++) {
            if (grid.at(column_idx, row_idx) == 'O') {
                // Walk the boulder to the edge
                int boulder_idx = row_idx - 1;
                int to_move_to = row_idx;
                for (; 0 <= boulder_idx; --boulder_idx) {
                    auto next_cell = GridCell{(uint)boulder_idx, (uint)column_idx};
                    auto next_cell_value = grid.at(next_cell);

                    if (
                            next_cell_value == '\0' || // cannot move beyond grid
                            next_cell_value == 'O' || // cannot move beyond another boulder
                            next_cell_value == '#' // cannot move beyond square boulder
                    ) {
                        break;
                    } else if (next_cell_value == '.') {
                        to_move_to--;
                        continue;
                    } else {
                        throw std::logic_error("The next cell must have a valid value");
                    }
                }

                // set initial position as space
                grid.set_value({(uint)row_idx, (uint)column_idx}, '.');
                // set destination position as the boulder ("move" it)
                // if the destination is the same as the initial one, it just doesn't move
                grid.set_value({(uint)to_move_to, (uint)column_idx}, 'O');
            }
        }
    }

    // 3. Rotate the grid back to point in whatever direction we initially wanted
    auto back_cw_rotations = (4 - cw_rotations) % 4;
    for (int rotation = 0; rotation < back_cw_rotations; rotation++) {
        grid.rotate_cw();
    }

    return grid;
}

static uint compute_load_on_direction(const Grid& in_grid, const WalkDirection& direction) {
    auto grid = in_grid; // Copy the grid

    // 1. Rotate the grid to point north, since I am lazy to actually solve properly for other directions
    auto cw_rotations = direction_to_cw_rotations.at(direction);
    for (int rotation = 0; rotation < cw_rotations; rotation++) {
        grid.rotate_cw();
    }

    // 2. Compute load
    ulong load_multiplier = grid.rows;
    ulong load = 0;
    for (int row_idx = 0; row_idx < grid.rows; row_idx++) {
        for (int column_idx = 0; column_idx < grid.columns; column_idx++) {
            if (grid.at(column_idx, row_idx) == 'O') {
                load += load_multiplier;
            }
        }
        load_multiplier--;
    }


    // 3. Rotate the grid back to point in whatever direction we initially wanted
    auto back_cw_rotations = (4 - cw_rotations) % 4;
    for (int rotation = 0; rotation < back_cw_rotations; rotation++) {
        grid.rotate_cw();
    }

    return load;
}

SOLVER(2023, 14, 1, true)
(const std::string &in) {
    auto grid = make_grid(in);
//    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    auto walked_boulders = walk_boulders(grid, NORTH);
    auto load = compute_load_on_direction(walked_boulders, NORTH);

    return fmt::format("{}", load);
}

SOLVER(2023, 14, 2, false)
(const std::string &in) {
//    auto grid = make_grid(in);
    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    constexpr const ulong CYCLES = 1'000'000'000;
    const std::vector<WalkDirection> directions = {NORTH, WEST, SOUTH, EAST};

    Grid walked_boulders = grid;
    uint current_cycle = 0;

    std::thread progress_thread = std::thread([&current_cycle, CYCLES]() {
        while (current_cycle < CYCLES) {
            auto percentage = ((double)current_cycle / (double)CYCLES) * 100;
            fmt::println("At cycle {} of {}, done {:.2f}%", current_cycle, CYCLES, percentage);
            std::this_thread::sleep_for(std::chrono::seconds (1));
        }
    });

    for (;current_cycle < CYCLES; current_cycle++) {
        for (const auto& direction: directions) {
            walked_boulders = walk_boulders(walked_boulders, direction);
        }
    }
    progress_thread.join();

    auto load = compute_load_on_direction(walked_boulders, NORTH);

    return fmt::format("{}", load);
}
