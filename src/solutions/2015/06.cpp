#include <regex>
#include "../one_solution.h"
#include "../grid.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
turn on 0,0 through 999,999
toggle 0,0 through 999,0
turn off 499,499 through 500,500
)";
#pragma endregion

namespace {
    enum InstructionType {
        TURN_OFF,
        TURN_ON,
        TOGGLE,
    };

    struct Instruction {
        InstructionType type;
        GridCell from;
        GridCell to;
    };
}

const std::map<std::string, InstructionType> string_to_instruction_type = {
        {"turn off", TURN_OFF},
        {"turn on", TURN_ON},
        {"toggle", TOGGLE},
};

static std::vector<Instruction> parse_instructions(const std::string &in) {
    auto lines_str = string_split(trim(in), '\n');
    std::vector<Instruction> instructions(lines_str.size());
    const std::regex regex(R"(^([^\d]+)(\d+),(\d+) through (\d+),(\d+))");
    std::transform(lines_str.begin(), lines_str.end(), instructions.begin(), [&regex](const auto &line_str) {
        std::smatch matches;
        if (!std::regex_match(line_str, matches, regex)) throw std::logic_error("The line must match the regex");

        auto instruction_type_str = trim(matches[1].str());
        auto instruction_type = string_to_instruction_type.at(instruction_type_str);
        GridCell first_coords{std::stol(matches[2].str()), std::stol(matches[3].str())};
        GridCell second_coords{std::stol(matches[4].str()), std::stol(matches[5].str())};

        return Instruction{instruction_type, first_coords, second_coords};
    });

    return instructions;
}

SOLVER(2015, 6, 1, false)
(const std::string &in) {
//    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_1));
    auto instructions = parse_instructions(in);

    Grid grid = make_grid(1000, 1000, '.');
    for (const auto &instruction: instructions) {
        if (grid.underlying_idx(instruction.from) > grid.underlying_idx(instruction.to)) {
            throw std::logic_error("Inverse direction not implemented");
        }

        for (long column = instruction.from.column; column <= instruction.to.column; ++column) {
            for (long row = instruction.from.row; row <= instruction.to.row; ++row) {
                if (instruction.type == TURN_ON) {
                    grid.set_value(GridCell{row, column}, '#');
                } else if (instruction.type == TURN_OFF) {
                    grid.set_value(GridCell{row, column}, '.');
                } else if (instruction.type == TOGGLE) {
                    if (grid.at(column, row) == '.') {
                        grid.set_value(GridCell{row, column}, '#');
                    } else {
                        grid.set_value(GridCell{row, column}, '.');
                    }
                }
            }
        }
    }

    long counter = 0;
    for (long column = 0; column < grid.columns; ++column) {
        for (long row = 0; row < grid.rows; ++row) {
            if (grid.at(column, row) == '#') counter++;
        }
    }

    // 568658 too low, forgot inclusivity
    return fmt::format("{}", counter);
}

SOLVER(2015, 6, 2, true)
(const std::string &in) {
//    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_1));
//    auto instructions = parse_instructions(std::string("turn on 0,0 through 0,0"));
    auto instructions = parse_instructions(in);

    Grid grid = make_grid(1000, 1000, '\0');
    for (const auto &instruction: instructions) {
        if (grid.underlying_idx(instruction.from) > grid.underlying_idx(instruction.to)) {
            throw std::logic_error("Inverse direction not implemented");
        }

        for (long column = instruction.from.column; column <= instruction.to.column; ++column) {
            for (long row = instruction.from.row; row <= instruction.to.row; ++row) {
                char current_value = grid.at(column, row);
                if (current_value >= 120) {
                    fmt::println("warning! value nearing limit");
                }
                if (instruction.type == TURN_ON) {
                    grid.set_value(GridCell{row, column}, (char)(current_value + 1));
                } else if (instruction.type == TURN_OFF) {
                    if (current_value == 0) continue;
                    grid.set_value(GridCell{row, column}, (char)(current_value - 1));
                } else if (instruction.type == TOGGLE) {
                    grid.set_value(GridCell{row, column}, (char)(current_value + 2));
                }
            }
        }
    }

    long counter = 0;
    for (long column = 0; column < grid.columns; ++column) {
        for (long row = 0; row < grid.rows; ++row) {
            counter += (char)grid.at(column, row);
        }
    }

    // 18800085 too high
    // 17325717 with negative values - too low
    // 17836115 with patch for 0
    return fmt::format("{}", counter);
}
