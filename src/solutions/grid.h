#ifndef AOC_GRID_H
#define AOC_GRID_H

#include <string>

typedef struct GridCell {
    long row;
    long column;

    bool operator==(const GridCell& other) const;
    bool operator!=(const GridCell& other) const;
    bool operator<(const GridCell& other) const;

    GridCell& operator+=(const GridCell& rhs);
    friend GridCell operator+(GridCell lhs, const GridCell& rhs);

    GridCell& operator-=(const GridCell& rhs);
    friend GridCell operator-(GridCell lhs, const GridCell& rhs);

    GridCell operator*(long multiplier) const;
} GridCell;

typedef struct Grid {
    std::size_t rows;
    std::size_t columns;
    std::string data;

    [[nodiscard]]
    char at(const std::size_t& column, const std::size_t& row) const noexcept;

    [[nodiscard]]
    char at(const GridCell&) const noexcept;

    /**
     * Sets value of cell
     *
     * @param cell cell to set
     * @param c character to set to
     * @return true if the position is inside grid, false if it is outside
     */
    bool set_value(const GridCell& cell, const char& c);

    [[nodiscard]]
    std::string substr(const std::size_t& column, const std::size_t& length, const std::size_t& row) const noexcept;

    [[nodiscard]] bool contains(const GridCell& cell) const;

    void add_row(const std::string &row);
    void add_row(const char &character);

    void add_column(const std::string &row);
    void add_column(const char &character);

    void add_row_start(const std::string &row);
    void add_column_start(const std::string &row);

    void rotate_cw();

    [[nodiscard]] GridCell find_first(const char &c) const;

    [[nodiscard]] ulong underlying_idx(const GridCell& cell) const;
    [[nodiscard]] GridCell underlying_idx_to_cell(ulong idx) const;
} Grid;

Grid make_grid(const std::string &in);
Grid make_grid(std::size_t rows, std::size_t columns, char fill_char);
void draw_grid(const Grid& grid);
std::string draw_grid_to_string(const Grid& grid);

#endif
