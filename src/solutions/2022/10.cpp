#include <map>
#include <numeric>
#include "../one_solution.h"
#include "../grid.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
addx 15
addx -11
addx 6
addx -3
addx 5
addx -1
addx -8
addx 13
addx 4
noop
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx 5
addx -1
addx -35
addx 1
addx 24
addx -19
addx 1
addx 16
addx -11
noop
noop
addx 21
addx -15
noop
noop
addx -3
addx 9
addx 1
addx -3
addx 8
addx 1
addx 5
noop
noop
noop
noop
noop
addx -36
noop
addx 1
addx 7
noop
noop
noop
addx 2
addx 6
noop
noop
noop
noop
noop
addx 1
noop
noop
addx 7
addx 1
noop
addx -13
addx 13
addx 7
noop
addx 1
addx -33
noop
noop
noop
addx 2
noop
noop
noop
addx 8
noop
addx -1
addx 2
addx 1
noop
addx 17
addx -9
addx 1
addx 1
addx -3
addx 11
noop
noop
addx 1
noop
addx 1
noop
noop
addx -13
addx -19
addx 1
addx 3
addx 26
addx -30
addx 12
addx -1
addx 3
addx 1
noop
noop
noop
addx -9
addx 18
addx 1
addx 2
noop
noop
addx 9
noop
noop
noop
addx -1
addx 2
addx -37
addx 1
addx 3
noop
addx 15
addx -21
addx 22
addx -6
addx 1
noop
addx 2
addx 1
noop
addx -10
noop
noop
addx 20
addx 1
addx 2
addx 2
addx -6
addx -11
noop
noop
)";
#pragma endregion

namespace {
    enum InstructionType {
        NOOP,
        ADDX,
    };

    struct Instruction {
        InstructionType type;
        int value;
        ulong cycles;
    };

    struct CPU {
        const std::vector<Instruction> instructions;
        ulong cycle = 1;
        int x = 1;

        explicit CPU(const std::vector<Instruction> &instructions): instructions(instructions) {};

        [[nodiscard]] int get_x() const {
            return this->x;
        }

        [[nodiscard]] ulong get_cycle() const {
            return this->cycle;
        }

        [[nodiscard]] bool is_finished() const {
            return this->instruction_idx == instructions.size();
        }

        void tick() {
            auto current_instruction = instructions.at(instruction_idx);
            if (current_instruction.cycles == current_instruction_cycle) {
                // Execute instruction
                if (current_instruction.value != 0) {
                    this->x += current_instruction.value;
                }
                current_instruction_cycle = 1; // TODO Might need to be 1
                instruction_idx++;
            } else {
                current_instruction_cycle++;
            }
            cycle++;
        }
    private:
        ulong instruction_idx = 0;
        ulong current_instruction_cycle = 1;
    };
}

static std::map<std::string, InstructionType> string_to_instruction = {
        {"noop", NOOP},
        {"addx", ADDX},
};

static std::map<InstructionType, ulong> instruction_to_cycles = {
        {NOOP, 1},
        {ADDX, 2},
};

static std::vector<Instruction> parse_instructions(const std::string &in) {
    auto lines = string_split(trim(in), '\n');
    std::vector<Instruction> instructions(lines.size());
    std::transform(lines.begin(), lines.end(), instructions.begin(), [](auto const &instruction_str) {
        auto instruction_type_str = std::string(instruction_str.begin(), instruction_str.begin() + 4);
        auto instruction_type = string_to_instruction.at(instruction_type_str);
        Instruction instruction{
                instruction_type,
                0,
                instruction_to_cycles.at(instruction_type),
        };

        if (instruction_type == ADDX) {
            instruction.value = std::stoi(std::string(instruction_str.begin() + 5, instruction_str.end()));
        }

        return instruction;
    });

    return instructions;
}

SOLVER(2022, 10, 1, true)
(const std::string &in) {
    auto instructions = parse_instructions(in);
//    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_1));
//    auto instructions = parse_instructions("noop\naddx 3\naddx -5");

    CPU cpu(instructions);
    std::vector<int> strengths;
    while (!cpu.is_finished()) {
        if ((cpu.cycle + 20) % 40 == 0) {
            fmt::println("cycle={} x={} strength={}", cpu.cycle, cpu.x, cpu.cycle * cpu.x);
            strengths.push_back((int)cpu.cycle * cpu.x);
        }
        cpu.tick();
    }

    auto sum = std::accumulate(strengths.begin(), strengths.end(), 0l, [](auto acc, auto strength) {
        return acc + strength;
    });

    return fmt::format("{}", sum);
}

SOLVER(2022, 10, 2, true)
(const std::string &in) {
    auto instructions = parse_instructions(in);
//    auto instructions = parse_instructions(std::string(EXAMPLE_INPUT_1));
//    auto instructions = parse_instructions("noop\naddx 3\naddx -5");

    Grid screen = make_grid(6, 40, '.');

    CPU cpu(instructions);
    while (!cpu.is_finished()) {
        auto position = GridCell{(long)((cpu.cycle - 1) / 40), (long)((cpu.cycle - 1) % 40)};
        long sprite_center = cpu.x;

        std::set<GridCell> sprite = {
                GridCell{position.row, sprite_center - 1},
                GridCell{position.row, sprite_center},
                GridCell{position.row, sprite_center + 1},
        };

        if (sprite.contains(position)) {
            screen.set_value(position, '#');
        }

        cpu.tick();
    }

//    draw_grid(screen);

    return fmt::format("\n{}", draw_grid_to_string(screen));
}
