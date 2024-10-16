#include "../one_solution.h"

#include "../../trim.h"
#include "../grid.h"

#include <cctype>
#include <vector>
#include <map>
#include <optional>
#include <numeric>

typedef struct NumberSpanningCells {
    uint starting_col;
    uint length;
    uint row;
    uint value;
} NumberSpanningCells;

static auto cmp = [](NumberSpanningCells a, NumberSpanningCells b) {
    return a.row != b.row || a.starting_col != b.starting_col || a.length != b.length;
};

typedef struct Symbol {
    char character;
    uint row;
    uint column;
    std::set<NumberSpanningCells, decltype(cmp)> adjacent_numbers;
} Symbol;

std::optional<NumberSpanningCells> find_number_spanning_cells(const Grid& grid, const GridCell& cell) {
    if (std::isdigit(grid.at(cell.column, cell.row))) {
        std::size_t starting_col = cell.column;
        std::size_t ending_col = cell.column;

        // Find start
        while (true) {
            if (std::isdigit(grid.at(starting_col - 1, cell.row))) {
                starting_col--;
            } else {
                break;
            }
        }

        // Find end
        while (true) {
            if (std::isdigit(grid.at(ending_col + 1, cell.row))) {
                ending_col++;
            } else {
                break;
            }
        }

        auto length = ending_col - starting_col + 1;
        std::string value_str = grid.substr(starting_col, length, cell.row);
        uint value = std::stoul(value_str);
        return std::make_optional<NumberSpanningCells>({(uint)starting_col, (uint)length, (uint)cell.row, value});
    }

    return std::nullopt;
}

std::set<NumberSpanningCells, decltype(cmp)> scan_for_numbers_around(const Grid& grid, const GridCell& cell) {
    std::vector<GridCell> to_scan = {
            {cell.row - 1, cell.column - 1},
            {cell.row - 1, cell.column + 0},
            {cell.row - 1, cell.column + 1},
            {cell.row + 0, cell.column - 1},
            // {cell.row + 0, cell.column + 0},
            {cell.row + 0, cell.column + 1},
            {cell.row + 1, cell.column - 1},
            {cell.row + 1, cell.column + 0},
            {cell.row + 1, cell.column + 1},
    };

    std::set<NumberSpanningCells, decltype(cmp)> numbers(cmp);
    for (const auto& cell_to_scan: to_scan) {
        auto number = find_number_spanning_cells(grid, cell_to_scan);
        if (number.has_value()) {
            numbers.insert(number.value());
        }
    }

    return numbers;
}

std::vector<Symbol> find_symbols(const std::string &in) {
    auto trimmed = trim(in);
    std::size_t columns = trimmed.find('\n');
    if (columns == std::string::npos) {
        throw std::logic_error("Could not determine number of columns");
    }

    std::size_t rows = std::count(trimmed.begin(), trimmed.end(), '\n') + 1;
    Grid grid = {rows, columns, trimmed};

    std::vector<Symbol> symbols;

    for (uint row = 0; row < rows; row++) {
        for (uint column = 0; column < columns; column++) {
            const char c = grid.at(column, row);
            if (!std::isdigit(c) && c != '.' && !std::isspace(c)) {
                auto adjacent_numbers = scan_for_numbers_around(grid, {row, column});
                symbols.push_back({c, row, column, adjacent_numbers});
            }
        }
    }

    return symbols;
}

constexpr const std::string_view EXAMPLE_1 = R"(
467..114..
...*......
..35..633.
......#...
617*......
.....+.58.
..592.....
......755.
...$.*....
.664.598..
)";

SOLVER(2023, 3, 1, true)
(const std::string &in) {
//    auto symbols = find_symbols(std::string(EXAMPLE_1));
    auto symbols = find_symbols(std::string(in));

    ulong sum = 0;
    for (const auto &symbol: symbols) {
        if (!symbol.adjacent_numbers.empty()) {
            sum += std::accumulate(
                    symbol.adjacent_numbers.begin(),
                    symbol.adjacent_numbers.end(),
                    0ul,
                    [](auto acc, auto current) {
                        return acc + current.value;
                    }
            );
        }
    }

    return fmt::format("{}", sum);
}

SOLVER(2023, 3, 2, true)
(const std::string &in) {
//    auto symbols = find_symbols(std::string(EXAMPLE_1));
    auto symbols = find_symbols(std::string(in));

    ulong sum = 0;
    for (const auto &symbol: symbols) {
        if (symbol.character == '*' && symbol.adjacent_numbers.size() == 2) {
            sum += std::accumulate(
                    symbol.adjacent_numbers.begin(),
                    symbol.adjacent_numbers.end(),
                    1ul,
                    [](auto acc, auto current) {
                        return acc * current.value;
                    }
            );
        }
    }

    return fmt::format("{}", sum);
}
