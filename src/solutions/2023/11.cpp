#include <map>
#include <ranges>
#include <numeric>
#include "../one_solution.h"
#include "../../trim.h"
#include "../string_split.h"
#include "../grid.h"

constexpr const std::string_view BEFORE_EXPANSION = R"(
...#......
.......#..
#.........
..........
......#...
.#........
.........#
..........
.......#..
#...#.....
)";
constexpr const std::string_view AFTER_EXPANSION = R"(
....#........
.........#...
#............
.............
.............
........#....
.#...........
............#
.............
.............
.........#...
#....#.......
)";

struct expand_universe_return {
    std::set<uint> columns_to_expand;
    std::set<uint> rows_to_expand;
};

expand_universe_return expand_universe(const std::string &in) {
    auto lines = string_split(trim(in), '\n');
    const std::size_t column_count = lines.at(0).length();
    const std::size_t row_count = lines.size();

    std::set<uint> columns_to_expand;
    std::set<uint> rows_to_expand;

    for (std::size_t column = 0; column < column_count; column++) {
        bool should_expand = true;
        for (std::size_t row = 0; row < row_count; row++) {
            if (lines[row][column] == '#') {
                should_expand = false;
                break;
            }
        }

        if (should_expand) {
            columns_to_expand.insert(column);
        }
    }

     for (std::size_t row_idx = 0; row_idx < row_count; row_idx++) {
         auto row = lines[row_idx];
         auto no_galaxies = std::all_of(row.begin(), row.end(), [](const char& c) {
             return c == '.';
         });

         if (no_galaxies) {
             rows_to_expand.insert(row_idx);
         }
     }

     return {columns_to_expand, rows_to_expand};
}

std::set<GridCell> find_galaxies(const Grid &galaxy, const struct expand_universe_return &expansion, uint expansion_modifier = 1) {
    // TODO This is probably a side effect assuming it being 1 from the first part
    if (expansion_modifier > 1) expansion_modifier -= 1;

    std::set<GridCell> galaxies;

    uint column_expansion = 0;
    uint row_expansion = 0;

    for (uint row = 0; row < galaxy.rows; row++) {
        if (expansion.rows_to_expand.contains(row)) row_expansion += expansion_modifier;

        for (uint column = 0; column < galaxy.columns; column++) {
            if (expansion.columns_to_expand.contains(column)) column_expansion += expansion_modifier;

            if (galaxy.at(column, row) == '#') {
                auto cell = GridCell{row + row_expansion, column + column_expansion};
                galaxies.insert(cell);
            }
        }
        column_expansion = 0;
    }

    return galaxies;
}

std::vector<std::pair<GridCell, GridCell>> make_galaxy_pairs(const std::set<GridCell>& galaxies) {
    std::vector<std::pair<GridCell, GridCell>> galaxy_pairs;

    for (auto it1 = galaxies.begin(); it1 != galaxies.end(); ++it1) {
        for (auto it2 = std::next(it1); it2 != galaxies.end(); ++it2) {
            galaxy_pairs.emplace_back(*it1, *it2);  // Add the pair
        }
    }

    return galaxy_pairs;
}

ulong find_shortest_distance(const GridCell &a, const GridCell &b) {
    auto smaller_row = std::min(a.row, b.row);
    auto bigger_row = std::max(a.row, b.row);
    auto smaller_column = std::min(a.column, b.column);
    auto bigger_column = std::max(a.column, b.column);

    return (bigger_column - smaller_column) + (bigger_row - smaller_row);
}

SOLVER(2023, 11, 1, true)
(const std::string &in) {
//    auto expansion = expand_universe(std::string(BEFORE_EXPANSION));
//    auto galaxy_grid = make_grid(std::string(BEFORE_EXPANSION));

    auto expansion = expand_universe(in);
    auto galaxy_grid = make_grid(in);
    auto galaxies = find_galaxies(galaxy_grid, expansion);
    auto pairs = make_galaxy_pairs(galaxies);

    std::vector<ulong> distances(pairs.size());
    std::transform(pairs.begin(), pairs.end(), distances.begin(), [](const auto& pair) {
        return find_shortest_distance(pair.first, pair.second);
    });

    ulong sum = std::accumulate(distances.begin(), distances.end(), 0ul, [](auto acc, auto val) {
        return acc + val;
    });

    return fmt::format("{}", sum);
}

SOLVER(2023, 11, 2, true)
(const std::string &in) {
//    auto expansion = expand_universe(std::string(BEFORE_EXPANSION));
//    auto galaxy_grid = make_grid(std::string(BEFORE_EXPANSION));

    auto expansion = expand_universe(in);
    auto galaxy_grid = make_grid(in);
    auto galaxies = find_galaxies(galaxy_grid, expansion, 1'000'000);
    auto pairs = make_galaxy_pairs(galaxies);

    std::vector<ulong> distances(pairs.size());
    std::transform(pairs.begin(), pairs.end(), distances.begin(), [](const auto& pair) {
        return find_shortest_distance(pair.first, pair.second);
    });

    ulong sum = std::accumulate(distances.begin(), distances.end(), 0ul, [](auto acc, auto val) {
        return acc + val;
    });

    return fmt::format("{}", sum);
}
