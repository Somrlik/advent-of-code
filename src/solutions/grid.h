#ifndef AOC_GRID_H
#define AOC_GRID_H

#include <string>

typedef struct GridCell {
    uint row;
    uint column;

    bool operator==(const GridCell& other) const;
    bool operator!=(const GridCell& other) const;
    bool operator<(const GridCell& other) const;
} GridCell;

typedef struct GridMathVector {
    short row;
    short column;
} GridMathVector;

typedef struct Grid {
    std::size_t rows;
    std::size_t columns;
    std::string data;
    bool lines_contain_newlines = true;

    [[nodiscard]]
    char at(const std::size_t& column, const std::size_t& row) const noexcept;

    [[nodiscard]]
    char at(const GridCell&) const noexcept;

    void set_value(const GridCell& cell, const char& c);

    [[nodiscard]]
    std::string substr(const std::size_t& column, const std::size_t& length, const std::size_t& row) const noexcept;
} Grid;

Grid make_grid(const std::string &in);
void draw_grid(const Grid& grid);

#endif
