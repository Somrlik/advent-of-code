#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"
#include <algorithm>
#include <map>
#include <ranges>
#include <numeric>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
???.### 1,1,3
.??..??...?##. 1,1,3
?#?#?#?#?#?#?#? 1,3,1,6
????.#...#... 4,1,1
????.######..#####. 1,6,5
?###???????? 3,2,1
)";

typedef struct Line {
    std::vector<char> symbols;
    std::vector<uint> instructions;
} Line;

typedef struct BruteForceGroup {
    char symbol;
    std::size_t length;
} BruteForceGroup;

static bool does_solution_work(const std::string &solution, const Line &line);

static std::vector<Line> parse_lines(const std::string &in) {
    auto lines_str = string_split(trim(in), '\n');
    std::vector<Line> lines(lines_str.size());
    std::transform(lines_str.begin(), lines_str.end(), lines.begin(), [](const auto& line_str) {
        auto symbols_and_instructions = string_split(trim(line_str), ' ');
        auto symbols_str = trim(symbols_and_instructions.at(0));
        std::vector<char> symbols(symbols_str.length());
        std::transform(symbols_str.begin(), symbols_str.end(), symbols.begin(), [](auto const &c) {
            return c;
        });

        auto instructions_str = string_split(trim(symbols_and_instructions.at(1)), ',');
        std::vector<uint> instructions(instructions_str.size());
        std::transform(instructions_str.begin(), instructions_str.end(), instructions.begin(), [](auto const &ins) {
            return std::stoi(ins);
        });

        return Line{symbols, instructions};
    });

    return lines;
}

typedef struct BruteForceCacheItem {
    uint total_permutations{0};
    std::vector<std::string> permutations;
} BruteForceCacheItem;

static std::map<std::size_t, BruteForceCacheItem> brute_force_cache;

static void brute_force_precompute_cache(std::size_t group_size) {
    if (brute_force_cache.contains(group_size)) {
        return;
    }

    auto pairs = std::views::iota(0, (int)group_size + 1)
                 | std::views::transform([group_size](int i) {
        return std::pair{(ulong)i, group_size - i};
    });

    for (auto const &[filled_count, space_count]: pairs) {
        // "#.?" is lexical
        std::string permutation;
        permutation.reserve(group_size);
        permutation.insert(0, filled_count, '#');
        permutation.insert(filled_count, space_count, '.');

        do {
            brute_force_cache[group_size].permutations.push_back(permutation);
        } while (std::next_permutation(permutation.begin(), permutation.end()));
    }

    brute_force_cache[group_size].total_permutations = brute_force_cache[group_size].permutations.size();
}

static uint determine_line_possibilities_brute_force(const Line &line) {
    std::vector<BruteForceGroup> groups;
    BruteForceGroup current = {line.symbols.at(0), 0};

    // Split into groups of symbols
    for (auto const &symbol: line.symbols) {
        if (current.symbol == symbol) {
            current.length++;
        } else {
            if (current.length == 0) continue;
            groups.push_back(current);
            current.length = 1;
            current.symbol = symbol;
        }
    }
    groups.push_back(current);

    struct PermutationIndex {
        uint current;
        std::size_t max;
    };

    std::map<uint, PermutationIndex> permutation_indexes;

    uint permutation_idx = 0;
    for (const auto &group: groups) {
        if (group.symbol == '?') {
            brute_force_precompute_cache(group.length);
            permutation_indexes[permutation_idx++] = {
                    0,
                    brute_force_cache[group.length].total_permutations - 1
            };
        }
    }

    // Accumulator
    std::string acc;
    acc.reserve(line.symbols.size());
    ulong acc_pos = 0;
    ulong possibility_count = 0;

    while (true) {
        permutation_idx = 0;
        for (const auto &group : groups) {
            if (group.symbol == '?') {
                // Determine which permutation to use
                auto current_permutation_index_group = permutation_indexes.at(permutation_idx++);
                auto to_insert = brute_force_cache[group.length].permutations[current_permutation_index_group.current];
                acc.insert(acc_pos, to_insert);
            } else {
                acc.insert(acc_pos, group.length, group.symbol);
            }

            acc_pos += group.length;
        }

        if (does_solution_work(acc, line)) {
            possibility_count++;
        }

        acc.erase();
        acc_pos = 0;

        // Determine which permutation to use for next iteration
        bool rollover = false;
        for (int p_idx = (int)permutation_indexes.size() - 1; p_idx >= 0; p_idx--) {
            auto permutation_index = &permutation_indexes.at(p_idx);

            // If max, rollover
            if (permutation_index->current == permutation_index->max) {
                if (p_idx == 0) {
                    rollover = true;
                    break;
                }
                permutation_index->current = 0;
                continue;
            }

            // Add one
            permutation_index->current++;
            break;
        }

        // If rollover on first, end the whole loop
        if (rollover) {
            break;
        }
    }

    return possibility_count;
}

static bool does_solution_work(const std::string &solution, const Line &line) {
    uint instruction_idx = 0;
    uint filled_counter = 0;
    bool in_filled = false;

    for (const auto& c: solution) {
        if (in_filled) {
            if (c == '#') {
                filled_counter++;
                if (instruction_idx == line.instructions.size()) {
                    return false;
                } else if (filled_counter > line.instructions.at(instruction_idx)) {
                    return false;
                } else {
                    // noop
                }
            } else {
                if (instruction_idx == line.instructions.size()) {
                    // If looking for more instruction and they aren't there, it is a bad solution
                    return false;
                }

                if (filled_counter != line.instructions.at(instruction_idx)) {
                    // If in filled and the count doesn't match instruction it is a wrong solution
                    return false;
                }

                instruction_idx++;
                in_filled = false;
                filled_counter = 0;
            }
        } else {
            if (c == '#') {
                in_filled = true;
                filled_counter++;
            } else {
                // noop
            }
        }
    }

    if (filled_counter == 0) {
        return instruction_idx == line.instructions.size();
    } else {
        return (instruction_idx == line.instructions.size() - 1) && (filled_counter == line.instructions.at(instruction_idx));
    }
}

uint determine_line_possibilities(const Line &line) {
    // TODO This will work, the brute solution is very bad
    int possibilities = 0;
    int last_space_idx = 0;
    for (auto const &instruction: line.instructions) {
        auto instruction_acc = instruction;

        // #?# 1,1 = one possibility
        // #.# = one possibility
        // ?.? 1,1 = one possibility

        for (int symbol_idx = last_space_idx; symbol_idx < line.symbols.size(); symbol_idx++) {
            auto const symbol = line.symbols[symbol_idx];
            if (symbol == '?') {

            } else if (symbol == '.') {
                last_space_idx = symbol_idx;
                // If instruction has been completed, continue to next instruction
                if (instruction_acc == 0) {
                    continue;
                }
            } else if (symbol == '#') {

            }
        }

    }

    return 0;
}

SOLVER(2023, 12, 1, true)
(const std::string &in) {
//    auto lines = parse_lines(std::string(EXAMPLE_INPUT_1));
     auto lines = parse_lines(in);

    std::vector<ulong> possibilities_counts(lines.size());
    std::transform(lines.begin(), lines.end(), possibilities_counts.begin(), [](const auto &line) {
        return determine_line_possibilities_brute_force(line);
    });

    ulong possibilities_sum = std::accumulate(possibilities_counts.begin(), possibilities_counts.end(), 0ul, [](auto acc, auto n) {
        return acc + n;
    });

    return fmt::format("{}", possibilities_sum);
}

SOLVER(2023, 12, 2, false)
(const std::string &in) {
    return "n/a";
}
