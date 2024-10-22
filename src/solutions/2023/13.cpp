#include <numeric>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
#.##..##.
..#.##.#.
##......#
##......#
..#.##.#.
..##..##.
#.#.##.#.

#...##..#
#....#..#
..##..###
#####.##.
#####.##.
..##..###
#....#..#
)";

namespace {
    typedef struct Map {
        uint row_count{0};
        uint column_count{0};
        std::vector<std::string> rows;
        std::vector<std::string> columns;
    } Map;
}

static std::vector<Map> parse_maps(const std::string &in) {
    auto maps_str = string_split(trim(in), "\n\n");
    std::vector<Map> maps(maps_str.size());
    std::transform(maps_str.begin(), maps_str.end(), maps.begin(), [](const auto& map_str) {
        auto rows = string_split(trim(map_str), '\n');
        std::size_t row_width = rows.at(0).size();
        std::vector<std::string> columns;

        for (uint column_idx = 0; column_idx < row_width; column_idx++) {
            std::string column;
            column.reserve(row_width);
            for (uint row_idx = 0; row_idx < rows.size(); row_idx++) {
                column.push_back(rows[row_idx][column_idx]);
            }
            columns.push_back(column);
        }

        return Map{
                (uint)rows.size(),
                (uint)row_width,
                rows,
                columns,
        };
    });

    return maps;
}

static uint solve_reflections(std::vector<std::string> sequence) {
    if (sequence.size() <= 2) {
        throw std::logic_error("The sequence must have at least 2 items");
    }

    // Try to find two identical in sequence next to each other
    for (uint sequence_idx = 0; sequence_idx < (sequence.size() - 1); sequence_idx++) {
        auto const& first = sequence.at(sequence_idx);
        auto const& second = sequence.at(sequence_idx + 1);

        if (first == second) {
            // Now check the reflection of other rows
            bool reflection_good = true;
            int left_pos = (int)sequence_idx - 1;
            int right_pos = (int)sequence_idx + 2;
            while (left_pos >= 0 && right_pos < sequence.size()) {
                if (sequence.at(left_pos--) == sequence.at(right_pos++)) {
                    continue;
                } else {
                    reflection_good = false;
                    break;
                }
            }

            // If reflection was good, return the sequence number
            if (reflection_good) {
                return sequence_idx + 1;
            }
        }
    }

    return 0;
}

static uint solve_map(const Map& map) {
    auto row_reflection = solve_reflections(map.rows);
    if (row_reflection > 0) {
        return row_reflection * 100;
    }

    auto column_reflection = solve_reflections(map.columns);
    if (column_reflection > 0) {
        return column_reflection;
    }

    return 0;
}

SOLVER(2023, 13, 1, true)
(const std::string &in) {
    auto maps = parse_maps(in);
//    auto maps = parse_maps(std::string(EXAMPLE_INPUT_1));

    std::vector<uint> solutions(maps.size());
    std::transform(maps.begin(), maps.end(), solutions.begin(), [](auto const& map) {
        return solve_map(map);
    });

    auto solutions_sum = std::accumulate(solutions.begin(), solutions.end(), 0ul, [](auto acc, auto solution) {
        return acc + solution;
    });

    return fmt::format("{}", solutions_sum);
}

static bool do_strings_differ_in_exactly_one_spot(
        const std::string &left,
        const std::string &right,
        int &the_spot
) {
    if (left.size() != right.size()) return false;

    int diff_index = -1;
    int diff_count = 0;

    for (int idx = 0; idx < left.length(); idx++) {
        if (left[idx] != right[idx]) {
            diff_count++;
            if (diff_count > 1) {
                // If more than one diff is found
                return false;
            }
            diff_index = idx;
        }
    }

    // If no diff is found
    if (diff_count != 1) {
        return false;
    }

    the_spot = diff_index;
    return true;
}

namespace {
    typedef struct fix_sequence_return {
        int column{-1};
        int first_similar{0};
        int second_similar{0};
    } fix_sequence_return;
}

fix_sequence_return fix_sequence(const std::vector<std::string> &sequence) {
    if (sequence.size() <= 2) {
        throw std::logic_error("The sequence must have at least 2 items");
    }

    // Try to find two identical in sequence next to each other
    for (uint sequence_idx = 0; sequence_idx < (sequence.size() - 1); sequence_idx++) {
        auto const& first = sequence.at(sequence_idx);
        auto const& second = sequence.at(sequence_idx + 1);

        if (first == second) {
            // Now check the reflection of other rows
            bool reflection_good = true;
            int left_pos = (int)sequence_idx - 1;
            int right_pos = (int)sequence_idx + 2;

            while (left_pos >= 0 && right_pos < sequence.size()) {
                const auto& left = sequence.at(left_pos--);
                const auto& right = sequence.at(right_pos++);

                int the_spot;

                if (do_strings_differ_in_exactly_one_spot(left, right, the_spot)) {
                    return {
                            the_spot,
                            left_pos + 1,
                            right_pos - 1
                    };
                } else if (left == right) {
                    continue;
                } else {
                    break;
                }
            }
        }
    }

    return {-1, -1, -1};
}

Map fix_map(const Map &map) {
    auto map_copy = map;

    auto row_fix = fix_sequence(map.rows);
    if (row_fix.column != -1) {
        map_copy.rows[row_fix.first_similar][row_fix.column] = map_copy.rows[row_fix.second_similar][row_fix.column];
        // TODO check that the fix is applied properly - it is most likely not
        map_copy.columns[row_fix.column][row_fix.first_similar] = map_copy.rows[row_fix.second_similar][row_fix.column];
        return map_copy;
    }

    auto column_fix = fix_sequence(map.columns);
    if (column_fix.column != -1) {
        map_copy.columns[column_fix.first_similar][column_fix.column] = map_copy.columns[column_fix.second_similar][column_fix.column];
        // TODO check that the fix is applied properly - it is most likely not
        map_copy.rows[column_fix.column][column_fix.first_similar] = map_copy.columns[column_fix.second_similar][column_fix.column];
        return map_copy;
    }

    throw std::logic_error("There must be something to fix");
}

SOLVER(2023, 13, 2, false)
(const std::string &in) {
//    auto maps = parse_maps(in);
    auto maps = parse_maps(std::string(EXAMPLE_INPUT_1));

    std::vector<Map> fixed_maps(maps.size());
    std::transform(maps.begin(), maps.end(), fixed_maps.begin(), [](auto const &map) {
        return fix_map(map);
    });

    std::vector<uint> solutions(maps.size());
    std::transform(maps.begin(), maps.end(), solutions.begin(), [](auto const& map) {
        return solve_map(map);
    });

    auto solutions_sum = std::accumulate(solutions.begin(), solutions.end(), 0ul, [](auto acc, auto solution) {
        return acc + solution;
    });

    return fmt::format("{}", solutions_sum);
}
