#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"
#include <stack>
#include <map>
#include <regex>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
    [D]
[N] [C]
[Z] [M] [P]
 1   2   3

move 1 from 2 to 1
move 3 from 1 to 3
move 2 from 2 to 1
move 1 from 1 to 2
)";

namespace {
    struct Stack {
        int id;
        std::stack<char> characters;
    };

    struct Instruction {
        int from;
        int to;
        int amount;
    };

    struct parse_input_return {
        std::map<int, Stack> stacks;
        std::vector<Instruction> instructions;
    };
}

static parse_input_return parse_input(const std::string &in) {
    auto const stacks_instructions_str = string_split(trim_only_newlines(in), "\n\n");

    // Stacks
    auto stacks_str = string_split(stacks_instructions_str.at(0), '\n');
    auto stack_count = std::stol(*(string_split(trim(*(stacks_str.end() - 1)), ' ').end() - 1));
    std::map<int, Stack> stacks;
    for (int stack_idx = 0; stack_idx < stack_count; ++stack_idx) {
        stacks.insert({stack_idx + 1, {stack_idx + 1, {}}});
    }
    stacks_str.erase(stacks_str.end());

    std::for_each(stacks_str.crbegin(), stacks_str.crend(), [&stack_count, &stacks](auto &line) {
        for (int stack_idx = 0; stack_idx < stack_count; ++stack_idx) {
            auto line_idx = 1 + (stack_idx * 4);
            if (line_idx >= line.size()) break;

            auto character = line.at(line_idx);
            if (character == ' ') continue;
            stacks.at(stack_idx + 1).characters.push(character);
        }
    });

    // Instructions
    std::regex regex(R"(move (\d+) from (\d+) to (\d+))");
    std::vector<Instruction> instructions;
    for (const auto &instruction_str: string_split(stacks_instructions_str.at(1), '\n')) {
        if (instruction_str.empty()) continue;

        std::smatch raw_matches;
        std::regex_search(instruction_str, raw_matches, regex);

        instructions.push_back(Instruction{
            std::stoi(raw_matches[2].str()),
            std::stoi(raw_matches[3].str()),
            std::stoi(raw_matches[1].str())}
        );
    }

    return {stacks, instructions};
}

static void perform_instructions(const std::vector<Instruction>& instructions, std::map<int, Stack> &stacks) {
    for (const auto &instruction: instructions) {
        for (int i = 0; i < instruction.amount; ++i) {
            auto from = &stacks.at(instruction.from).characters;
            auto to = &stacks.at(instruction.to).characters;

            if (from->empty()) throw std::logic_error("A stack cannot be empty");

            auto character = from->top();
            from->pop();
            to->push(character);
        }
    }
}

SOLVER(2022, 5, 1, true)
(const std::string &in) {
    auto [stacks, instructions] = parse_input(in);
//    auto [stacks, instructions] = parse_input(std::string(EXAMPLE_INPUT_1));
    perform_instructions(instructions, stacks);

    std::string acc;
    acc.reserve(stacks.size());
    for (const auto &[_, stack]: stacks) {
        acc += stack.characters.top();
    }

    return fmt::format("{}", acc);
}

static void perform_instructions_improved(const std::vector<Instruction>& instructions, std::map<int, Stack> &stacks) {
    for (const auto &instruction: instructions) {
        std::vector<char> moving_arm;

        for (int i = 0; i < instruction.amount; ++i) {
            auto from = &stacks.at(instruction.from).characters;
            if (from->empty()) throw std::logic_error("A stack cannot be empty");
            moving_arm.push_back(from->top());
            from->pop();
        }

        auto to = &stacks.at(instruction.to).characters;
        std::for_each(moving_arm.crbegin(), moving_arm.crend(), [&to](auto val) {
            to->push(val);
        });
    }
}

SOLVER(2022, 5, 2, true)
(const std::string &in) {
    auto [stacks, instructions] = parse_input(in);
//    auto [stacks, instructions] = parse_input(std::string(EXAMPLE_INPUT_1));
    perform_instructions_improved(instructions, stacks);

    std::string acc;
    acc.reserve(stacks.size());
    for (const auto &[_, stack]: stacks) {
        acc += stack.characters.top();
    }

    return fmt::format("{}", acc);
}
