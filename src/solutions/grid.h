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

    [[nodiscard]]
    char at(const std::size_t& column, const std::size_t& row) const noexcept;

    [[nodiscard]]
    char at(const GridCell&) const noexcept;

    void set_value(const GridCell& cell, const char& c);

    [[nodiscard]]
    std::string substr(const std::size_t& column, const std::size_t& length, const std::size_t& row) const noexcept;

    void add_row(const std::string &row);
    void add_column(const std::string &row);

    void add_row_start(const std::string &row);
    void add_column_start(const std::string &row);

    void rotate_cw();
} Grid;

Grid make_grid(const std::string &in);
void draw_grid(const Grid& grid);

#endif
