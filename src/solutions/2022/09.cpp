#include "../one_solution.h"
#include "../grid.h"
#include "../string_split.h"
#include "../../trim.h"
#include <map>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
R 4
U 4
L 3
D 1
R 4
D 1
L 5
R 2
)";

constexpr const std::string_view EXAMPLE_INPUT_2 = R"(
R 5
U 8
L 8
D 3
R 17
D 10
L 25
U 20
)";

namespace {
    enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT,
    };

    struct Instruction {
        Direction direction;
        int count;
    };
}

static std::map<Direction, GridCell> direction_to_vector = {
        {UP, {-1, 0}},
        {RIGHT, {0, +1}},
        {DOWN, {+1, 0}},
        {LEFT, {0, -1}},
};

static std::map<char, Direction> char_to_direction = {
        {'U', UP},
        {'R', RIGHT},
        {'D', DOWN},
        {'L', LEFT},
};

static std::vector<Instruction> parse_instructions(const std::string &in) {
    auto instructions_str = string_split(trim(in), '\n');
    std::vector<Instruction> instructions;
    for (auto const &instruction_str: instructions_str) {
        auto direction = char_to_direction.at(instruction_str.at(0));
        auto count = std::stoi(std::string(instruction_str.begin() + 2, instruction_str.end()));
        instructions.emplace_back(Instruction{direction, count});
    }

    return instructions;
}

static void simulate_step(GridCell &head, GridCell &tail) {
    // Move tail
    long movement_row = 0;
    long movement_column = 0;
    if (std::abs(head.column - tail.column) >= 2) {
        if (std::abs(head.row - tail.row) == 1) {
            // move diagonally towards head
            movement_row = head.row - tail.row;
        }

        // move directly towards head
        movement_column = head.column - tail.column;
    } else if (std::abs(head.row - tail.row) >= 2) {
        if (std::abs(head.column - tail.column) == 1) {
            // move diagonally towards head
            movement_column = head.column - tail.column;
        }

        // move directly towards head
        movement_row = head.row - tail.row;
    }

    auto movement_vector = GridCell{std::clamp(movement_row, -1l, 1l), std::clamp(movement_column, -1l, 1l)};
    tail += movement_vector;
}

SOLVER(2022, 9, 1, true)
(const std::string &in) {
    auto instructions = parse_instructions(in);
//    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_1));

    std::set<GridCell> visited_cells;

    GridCell head{0, 0};
    GridCell tail{0, 0};

    for (const auto &instruction: instructions) {
        for (int counter = 0; counter < instruction.count; ++counter) {
            head += direction_to_vector.at(instruction.direction);
            simulate_step(head, tail);
            visited_cells.insert(tail);
        }
    }

    return fmt::format("{}", visited_cells.size());
}

SOLVER(2022, 9, 2, false)
(const std::string &in) {
//    auto instructions = parse_instructions(in);
    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_2));

    std::set<GridCell> visited_cells;

    const int PARTS_COUNT = 10;

    std::vector<GridCell> parts(10, {0, 0});

    for (const auto &instruction: instructions) {
        for (int counter = 0; counter < instruction.count; ++counter) {
            parts.at(0) += direction_to_vector.at(instruction.direction);
            for (int part_counter = 0; part_counter < (PARTS_COUNT - 1); ++part_counter) {
                simulate_step(parts.at(part_counter), parts.at(part_counter + 1));
            }
            visited_cells.insert(parts.at(PARTS_COUNT - 1));
        }
    }

    // 2792 too high
    return fmt::format("{}", visited_cells.size());
}
