#include <stdexcept>
#include "grid.h"

#include "./string_split.h"
#include "../trim.h"
#include "fmt/format.h"

// Logic ops
bool GridCell::operator==(const GridCell& other) const {
    return (row == other.row) && (column == other.column);
}

bool GridCell::operator!=(const GridCell& other) const {
    return !operator==(other);
}

bool GridCell::operator<(const GridCell &other) const {
    if (row != other.row) return row < other.row;
    return column < other.column;
}

// Addition
GridCell &GridCell::operator+=(const GridCell &rhs) {
    this->column += rhs.column;
    this->row += rhs.row;
    return *this;
}

GridCell operator+(GridCell lhs, const GridCell &rhs) {
    return GridCell{lhs.row + rhs.row, lhs.column + rhs.column};
}

// Subtraction
GridCell &GridCell::operator-=(const GridCell &rhs) {
    this->column -= rhs.column;
    this->row -= rhs.row;
    return *this;
}

GridCell operator-(GridCell lhs, const GridCell &rhs) {
    return GridCell{lhs.row - rhs.row, lhs.column - rhs.column};
}

// Multiplication
GridCell GridCell::operator*(long multiplier) const {
    return GridCell{row * multiplier, column * multiplier};
}


[[nodiscard]]
char Grid::at(const std::size_t& column, const std::size_t& row) const noexcept {
    if (column >= columns) {
        return '\0';
    }

    if (row >= rows) {
        return '\0';
    }

    return data.at(column + ((columns) * row));
}

[[nodiscard]]
char Grid::at(const GridCell& cell) const noexcept {
    return at(cell.column, cell.row);
}

[[nodiscard]]
std::string Grid::substr(const std::size_t& column, const std::size_t& length, const std::size_t& row) const noexcept {
    return data.substr(column + ((columns) * row), length);
}

bool Grid::set_value(const GridCell& cell, const char& c) {
    if (this->contains(cell)) {
        data[(cell.column + ((columns) * cell.row))] = c;
        return true;
    }
    return false;
}

void Grid::add_row(const std::string &row) {
    auto trimmed_row = trim(row);
    if (trimmed_row.size() != columns) {
        throw std::logic_error("The inserted row must be as long as the number of columns");
    }

    data.append(trimmed_row);
    rows++;
}

void Grid::add_row(const char &character) {
    std::string new_row(this->columns, character);
    this->add_row(new_row);
}

void Grid::add_column(const std::string &column) {
    auto trimmed_column = trim(column);
    if (trimmed_column.size() != rows) {
        throw std::logic_error("The inserted column must be as long as the number of rows");
    }

    uint row_idx = 0;
    for (char column_idx : trimmed_column) {
        if (row_idx >= (this->rows - 1)) {
            data.insert(data.end(), column_idx);
        } else {
            data.insert(columns + (columns * row_idx) + row_idx, 1, column_idx);
        }
        row_idx++;
    }
    columns++;
}

void Grid::add_column(const char &character) {
    std::string new_column(this->rows, character);
    this->add_column(new_column);
}

void Grid::add_row_start(const std::string &row) {
    auto trimmed_row = trim(row);
    if (trimmed_row.size() != columns) {
        throw std::logic_error("The inserted row must be as long as the number of columns");
    }

    data.insert(0, trimmed_row);
    rows++;
}

void Grid::add_column_start(const std::string &column) {
    auto trimmed_column = trim(column);
    if (trimmed_column.size() != rows) {
        throw std::logic_error("The inserted column must be as long as the number of rows");
    }

    uint row_idx = 0;
    for (char column_idx : trimmed_column) {
        data.insert(0 + ((columns + row_idx) * row_idx), 1, column_idx);
        row_idx++;
    }
    columns++;
}

void Grid::rotate_cw() {
    // It is probably(???) easier to rotate when represented as distinct rows and columns
    // It would be prescient to do it in-place but I'm drunk right now

    // Create 2 dimensional vector
    std::vector<std::vector<char>> split_grid(rows, std::vector<char>(columns));
    int idx = 0;
    for (int r = 0; r < rows; r++) {
        for (int c = 0; c < columns; c++) {
            split_grid[r][c] = data[idx++];
        }
    }

    // Create a rotated 2 dimensional vector
    std::vector<std::vector<char>> rotated_grid(columns, std::vector<char>(rows));
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < columns; ++c) {
            rotated_grid[c][rows - 1 - r] = split_grid[r][c];
        }
    }

    // Convert 2d to data string
    std::string new_data;
    for (int r = 0; r < columns; ++r) {
        for (int c = 0; c < rows; ++c) {
            new_data += rotated_grid[r][c];
        }
    }

    data = new_data;
    auto tmp = columns;
    columns = rows;
    rows = tmp;
}

bool Grid::contains(const GridCell &cell) const {
    return (0 <= cell.row && cell.row < this->rows) && (0 <= cell.column && cell.column < this->columns);
}

ulong Grid::underlying_idx(const GridCell &cell) const {
    return cell.column + (columns * cell.row);
}

GridCell Grid::underlying_idx_to_cell(ulong idx) const {
    return GridCell{(long)idx / ((long)columns), (long)idx % (long)columns};
}

GridCell Grid::find_first(const char &c) const {
    for (long row = 0; row < rows; ++row) {
        for (long column = 0; column < columns; ++column) {
            if (at(column, row) == c) return GridCell{row, column};
        }
    }
    throw std::out_of_range("character not found");
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
    
    return Grid{rows, columns, trimmed};
}

void draw_grid(const Grid &grid) {
    for (std::size_t row = 0; row < grid.rows; row++) {
        fmt::println("{}", grid.data.substr(0 + (row * grid.columns), grid.columns));
    }
    fmt::println("------------------------------------");
}

std::string draw_grid_to_string(const Grid& grid) {
    std::string str;
    for (std::size_t row = 0; row < grid.rows; row++) {
        str += fmt::format("{}\n", grid.data.substr(0 + (row * grid.columns), grid.columns));
    }
    return str;
}

Grid make_grid(std::size_t rows, std::size_t columns, char fill_char) {
    return Grid{rows, columns, std::string(rows * columns, fill_char)};
}
