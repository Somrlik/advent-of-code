#include "../one_solution.h"
#include "../grid.h"
#include "../../trim.h"
#include <map>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
..F7.
.FJ|.
SJ.L7
|F--J
LJ...
)";

typedef enum Direction {
    DIRECTION_NORTH = 0,
    DIRECTION_EAST = 1,
    DIRECTION_SOUTH = 2,
    DIRECTION_WEST = 3,
} Direction;

typedef struct DirectionConnections {
    std::vector<short> direction_vector;
    std::string connecting_chars;
    Direction required_previous_direction;
} DirectionConnections;

const std::vector<DirectionConnections> directions_connections = {
        {{-1, 0}, "|7F", DIRECTION_NORTH}, // top
        {{0, +1}, "-J7", DIRECTION_EAST}, // right
        {{+1, 0}, "|LJ", DIRECTION_SOUTH}, // bottom
        {{0, -1}, "-LF", DIRECTION_WEST}, // left
};

const std::map<char, std::vector<Direction>> possible_directions_from_char = {
        {'|', {DIRECTION_NORTH, DIRECTION_SOUTH}}, // | is a vertical pipe connecting north and south.
        {'-', {DIRECTION_EAST, DIRECTION_WEST}}, //- is a horizontal pipe connecting east and west.
        {'L', {DIRECTION_NORTH, DIRECTION_EAST}}, //L is a 90-degree bend connecting north and east.
        {'J', {DIRECTION_NORTH, DIRECTION_WEST}}, //J is a 90-degree bend connecting north and west.
        {'7', {DIRECTION_SOUTH, DIRECTION_WEST}}, //7 is a 90-degree bend connecting south and west.
        {'F', {DIRECTION_SOUTH, DIRECTION_EAST}}, //F is a 90-degree bend connecting south and east.
        {'.', {}},  //. is ground; there is no pipe in this tile.
        {'S', {}}, //S is the starting position of the animal; there is a pipe on this tile, but your sketch doesn't show what shape the pipe has.
};

bool is_one_of(const char& c, const std::string &option) {
    return option.find(c) != std::string::npos;
}

GridCell find_start(const Grid& grid) {
    for (uint column = 0; column < grid.columns; column++) {
        for (uint row = 0; row < grid.rows; row++) {
            if (grid.at(column, row) == 'S') {
                return GridCell{row, column};
            }
        }
    }

    throw std::logic_error("Could not find start!");
}

GridCell determine_next_cell(const Grid &grid, const GridCell& previous, const GridCell& current) {
    auto current_character = grid.at(current);
    auto current_possible_directions = possible_directions_from_char.at(current_character);

    for (const auto& connection: directions_connections) {
        auto cell = GridCell{current.row + connection.direction_vector.at(0), current.column + connection.direction_vector.at(1)};
        auto cell_character = grid.at(cell);

        // Skip if navigating back
        if (cell == previous) {
            continue;
        }

        if (is_one_of(cell_character, connection.connecting_chars)) {
            auto it = std::find(current_possible_directions.begin(), current_possible_directions.end(), connection.required_previous_direction);
            if (it != current_possible_directions.end()) {
                fmt::println("moving from {} to {}", current_character, cell_character);
                return cell;
            } else {
                fmt::println("FAILED TO MOVE from {} to {}", current_character, cell_character);
            }
        }

        if (cell_character == 'S') {
            fmt::println("!!! Got back to start! This probably shouldn't happen when going in opposite directions...");
        }
    }

    throw std::logic_error("There must a path to follow indefinitely");
}

std::vector<GridCell> determine_initial_paths(const Grid& grid, const GridCell& start) {
    std::vector<GridCell> paths;
    paths.reserve(2);

    for (const auto& connection: directions_connections) {
        auto cell = GridCell{start.row + connection.direction_vector.at(0), start.column + connection.direction_vector.at(1)};
        auto cell_character = grid.at(cell);
        if (is_one_of(cell_character, connection.connecting_chars)) {
            paths.push_back(cell);
        }
    }

    return paths;
}

SOLVER(2023, 10, 1, true)
(const std::string &in) {
    auto grid = make_grid(trim(in));
//    auto grid = make_grid(trim(std::string(EXAMPLE_INPUT_1)));

    auto grid_for_drawing = grid;

    auto start = find_start(grid);
    auto paths = determine_initial_paths(grid, start);
    if (paths.size() != 2) {
        throw std::logic_error("There must be exactly two paths");
    }

    long steps = 1; // We initially made a step from start to current
    std::vector<GridCell> previouses = {start, start};
    std::vector<GridCell> currents = {paths.at(0), paths.at(1)};

    std::vector<GridCell> second_path;
    second_path.push_back(start);

    while (true) {
        second_path.push_back(currents[1]);
        // First walk all turtles
        for (std::size_t i = 0; i < currents.size(); i++) {
            auto step_str = std::to_string(steps);
            grid_for_drawing.set_value(previouses[i], step_str.at(step_str.length() - 1));

            auto new_path = determine_next_cell(grid, previouses[i], currents[i]);
            previouses[i] = currents[i];
            currents[i] = new_path;
        }
        // We made a step
        steps++;

        if (currents.at(0) == currents.at(1)) {
            // We met
            break;
        }
    }

    return fmt::format("{}", steps);
}

SOLVER(2023, 10, 2, false)
(const std::string &in) {
    return "n/a";
}
