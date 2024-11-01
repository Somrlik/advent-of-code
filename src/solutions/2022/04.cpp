#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
2-4,6-8
2-3,4-5
5-7,7-9
2-8,3-7
6-6,4-6
2-6,4-8
)";

namespace {
    struct Shift {
        long start;
        long end;
    };

    typedef std::array<Shift, 2> ElfPair;
}

static std::vector<ElfPair> parse_pairs(const std::string& in) {
    auto elf_pairs_str = string_split(trim(in), '\n');
    std::vector<ElfPair> elf_pairs;
    for (auto const& elf_pair_str: elf_pairs_str) {
        auto shift_pair_str = string_split(trim(elf_pair_str), ',');
        if (shift_pair_str.size() != 2) throw std::logic_error("There must be exactly two shifts");

        auto first_shift = string_split(shift_pair_str.at(0), '-');
        if (first_shift.size() != 2) throw std::logic_error("There must be exactly two numbers");

        auto second_shift = string_split(shift_pair_str.at(1), '-');
        if (second_shift.size() != 2) throw std::logic_error("There must be exactly two numbers");

        elf_pairs.push_back(ElfPair{
            Shift{std::stol(first_shift.at(0)), std::stol(first_shift.at(1))},
            Shift{std::stol(second_shift.at(0)), std::stol(second_shift.at(1))},
        });
    }

    return elf_pairs;
}

static bool does_pair_overlap_fully(const ElfPair& pair) {
    auto pairs_by_length = pair;

    std::sort(pairs_by_length.begin(), pairs_by_length.end(), [](const auto& a, const auto& b) {
        return (a.end - a.start) > (b.end - b.start);
    });

    // Zeroth one is longer
    return (pairs_by_length.at(0).start <= pairs_by_length.at(1).start) &&
            (pairs_by_length.at(0).end >= pairs_by_length.at(1).end);
}

SOLVER(2022, 4, 1, true)
(const std::string &in) {
    auto elf_pairs = parse_pairs(in);
//    auto elf_pairs = parse_pairs(std::string(EXAMPLE_INPUT_1));

    long overlapping_pairs = 0;
    for (const auto &pair: elf_pairs) {
        if (does_pair_overlap_fully(pair)) {
            overlapping_pairs++;
        }
    }

    return fmt::format("{}", overlapping_pairs);
}

static bool does_pair_overlap_partially(const ElfPair& pair) {
    auto pairs_by_length = pair;

    std::sort(pairs_by_length.begin(), pairs_by_length.end(), [](const auto& a, const auto& b) {
        return (a.end - a.start) > (b.end - b.start);
    });

    // Zeroth one is longer
    return !(pairs_by_length.at(0).start > pairs_by_length.at(1).end ||
           pairs_by_length.at(0).end < pairs_by_length.at(1).start);
}

SOLVER(2022, 4, 2, true)
(const std::string &in) {
    auto elf_pairs = parse_pairs(in);
//    auto elf_pairs = parse_pairs(std::string(EXAMPLE_INPUT_1));

    long overlapping_pairs = 0;
    for (const auto &pair: elf_pairs) {
        if (does_pair_overlap_partially(pair)) {
            overlapping_pairs++;
        }
    }

    return fmt::format("{}", overlapping_pairs);
}
