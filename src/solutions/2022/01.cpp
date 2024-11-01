#include <numeric>
#include "../one_solution.h"
#include "../../trim.h"
#include "../string_split.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
1000
2000
3000

4000

5000
6000

7000
8000
9000

10000
)";

namespace {
    typedef struct Elf {
        long total;
    } Elf;
}

static std::vector<Elf> parse_elves(const std::string &in) {
    auto elves_str = string_split(trim(in), "\n\n");
    std::vector<Elf> elves(elves_str.size());
    for (auto const &elf_str: elves_str) {
        auto calories_str = string_split(trim(elf_str), '\n');
        long calories = 0;
        for (const auto& calorie_str: calories_str) {
            calories += std::stol(calorie_str);
        }
        elves.emplace_back(Elf{calories});
    }
    return elves;
}

SOLVER(2022, 1, 1, true)
(const std::string &in) {
    auto elves = parse_elves(in);
//    auto elves = parse_elves(std::string(EXAMPLE_INPUT_1));

    auto max = std::accumulate(elves.begin(), elves.end(), std::numeric_limits<long>::min(), [](auto acc, auto const& elf) {
        return std::max(acc, elf.total);
    });

    return fmt::format("{}", max);
}

SOLVER(2022, 1, 2, true)
(const std::string &in) {
    auto elves = parse_elves(in);
//    auto elves = parse_elves(std::string(EXAMPLE_INPUT_1));

    std::sort(elves.begin(), elves.end(), [](const auto &a, const auto &b) {
        return a.total > b.total;
    });

    long sum = elves.at(0).total + elves.at(1).total + elves.at(2).total;
    return fmt::format("{}", sum);
}
