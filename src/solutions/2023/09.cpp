#include <numeric>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
0 3 6 9 12 15
1 3 6 10 15 21
10 13 16 21 30 45
)";

typedef enum ExtrapolationDirection {
    EXTRAPOLATION_FIRST = 0,
    EXTRAPOLATION_LAST = -1,
} ExtrapolationDirection;

typedef std::vector<std::vector<long>> Input;

static Input parse_input(const std::string& in) {
    auto lines_str = string_split(trim(in), '\n');
    std::vector<std::vector<long>> lines(lines_str.size());
    std::transform(lines_str.begin(), lines_str.end(), lines.begin(), [](const auto &line) {
        auto numbers_str = string_split(trim(line), ' ');
        std::vector<long> numbers(numbers_str.size());
        std::transform(numbers_str.begin(), numbers_str.end(), numbers.begin(), [](const auto &number) {
            return std::stol(number);
        });
        return numbers;
    });
    return lines;
}

long determine_extrapolation(std::vector<long> numbers, ExtrapolationDirection direction) {
    std::vector<std::vector<long>> diffs = {{numbers}};
    long current_diff_idx = 0; // Will be negative
    while (true) {
        std::vector<long> next_diff;
        for (std::size_t i = 0; i < (diffs[current_diff_idx].size() - 1); i++) {
            next_diff.push_back(diffs[current_diff_idx][i + 1] - diffs[current_diff_idx][i]);
        }

        auto all_zeroes = std::all_of(next_diff.begin(), next_diff.end(), [](auto const& number) {
            return number == 0;
        });

        if (all_zeroes) break;

        diffs.push_back(next_diff);
        current_diff_idx++;
    }

    long acc = 0;
    while (current_diff_idx >= 0) {
        auto last_diff = diffs[current_diff_idx--]; //.at(direction == EXTRAPOLATION_LAST ? (diffs.size() - 1) : 0);
        if (direction == EXTRAPOLATION_LAST) {
            auto last = last_diff.at(last_diff.size() - 1);
            acc += last;
        } else {
            auto last = last_diff.at(0);
            acc = last - acc;
        }
    }
    return acc;
}

SOLVER(2023, 9, 1, true)
(const std::string &in) {
//    auto lines = parse_input(std::string(EXAMPLE_INPUT_1));
    auto lines = parse_input(in);
    std::vector<long> extrapolations(lines.size());
    std::transform(lines.begin(), lines.end(), extrapolations.begin(), [](auto const& line) {
        return determine_extrapolation(line, EXTRAPOLATION_LAST);
    });

    long sum = std::accumulate(extrapolations.begin(), extrapolations.end(), 0l, [](auto acc, auto num) {
        return acc + num;
    });
    return fmt::format("{}", sum);
}

SOLVER(2023, 9, 2, true)
(const std::string &in) {
    auto lines = parse_input(in);
    std::vector<long> extrapolations(lines.size());
    std::transform(lines.begin(), lines.end(), extrapolations.begin(), [](auto const& line) {
        return determine_extrapolation(line, EXTRAPOLATION_FIRST);
    });

    long sum = std::accumulate(extrapolations.begin(), extrapolations.end(), 0l, [](auto acc, auto num) {
        return acc + num;
    });
    return fmt::format("{}", sum);
}
