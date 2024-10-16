#include <stdexcept>
#include "grid.h"

#include "./string_split.h"
#include "../trim.h"
#include "fmt/format.h"

bool GridCell::operator==(const GridCell& other) const {
    return row == other.row && column == other.column;
}

bool GridCell::operator!=(const GridCell& other) const {
    return !operator==(other);
}

bool GridCell::operator<(const GridCell &other) const {
    if (row != other.row) return row < other.row;
    return column < other.column;
}

[[nodiscard]]
char Grid::at(const std::size_t& column, const std::size_t& row) const noexcept {
    if (column >= columns) {
        return '\0';
    }

    if (row >= rows) {
        return '\0';
    }

    // +1 is for \n
    return data.at(column + ((columns + (lines_contain_newlines ? 1 : 0)) * row));
}

[[nodiscard]]
char Grid::at(const GridCell& cell) const noexcept {
    return at(cell.column, cell.row);
}

[[nodiscard]]
std::string Grid::substr(const std::size_t& column, const std::size_t& length, const std::size_t& row) const noexcept {
    return data.substr(column + ((columns + 1) * row), length);
}

void Grid::set_value(const GridCell& cell, const char& c) {
    data[(cell.column + ((columns + (lines_contain_newlines ? 1 : 0)) * cell.row))] = c;
}

Grid make_grid(const std::string &in) {
    std::string trimmed = trim(in);
    auto lines = string_split(trimmed, '\n');
    if (lines.empty()) {
        throw std::logic_error("Grid must have at least 1 row");
    }
    ulong rows = lines.size();
    ulong columns = lines.at(0).size();

    trimmed.erase(std::remove_if(trimmed.begin(), trimmed.end(), [](const char& c) {
        return c == '\n';
    }), trimmed.end());
    
    return Grid{rows, columns, trimmed, false};
}

void draw_grid(const Grid &grid) {
    if (grid.lines_contain_newlines) {
        fmt::println("{}", grid.data);
    } else {
        for (std::size_t row = 0; row < grid.rows; row++) {
            fmt::println("{}", grid.data.substr(0 + (row * grid.columns), grid.columns));
        }
    }
    fmt::println("------------------------------------");
}
