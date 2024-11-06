#include "../one_solution.h"
#include "../grid.h"
#include "../Graph.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
Sabqponm
abcryxxl
accszExk
acctuvwj
abdefghi
)";
#pragma endregion

static Graph build_graph(const Grid& grid) {
    Graph graph(grid.columns * grid.rows);
    const std::array<GridCell, 4> directions = {
            GridCell{+1,  0},
            GridCell{ 0, +1},
            GridCell{-1,  0},
            GridCell{ 0, -1},
    };

    for (long row = 0; row < grid.rows; ++row) {
        for (long column = 0; column < grid.columns; ++column) {
            auto current_cell = GridCell{row, column};
            auto current_value = grid.at(current_cell);

            for (auto const& direction_vector: directions) {
                auto examined_cell = current_cell + direction_vector;
                auto examined_value = grid.at(examined_cell);

                if (examined_value == '\0') continue; // skip out of grid
                if (examined_value < current_value) {
                    graph.add_directed_edge(
                            (long) grid.underlying_idx(current_cell),
                            100'000, // going down is heavily penalized
                            (long) grid.underlying_idx(examined_cell)
                    );
                }
                if (examined_value == current_value) {
                    graph.add_directed_edge(
                            (long) grid.underlying_idx(current_cell),
                            1'000, // going in the same level is penalized
                            (long) grid.underlying_idx(examined_cell)
                    );
                }
                if (examined_value == (current_value + 1)) {
                    // Add an edge with cost of 1
                    graph.add_directed_edge(
                            (long) grid.underlying_idx(current_cell),
                            1, // going up is great
                            (long) grid.underlying_idx(examined_cell)
                    );
                }
            }
        }
    }

    return graph;
}

void draw_path(const Grid& grid, const Graph& graph, const std::vector<long>& parents) {
    auto grid_copy = grid;
    std::for_each(parents.cbegin(), parents.cend(), [&grid_copy, &graph](const auto& parent) {
        auto cell = grid_copy.underlying_idx_to_cell(parent);
        grid_copy.set_value(cell, '#');
        draw_grid(grid_copy);
    });
}

SOLVER(2022, 12, 1, true)(const std::string &in) {
    auto grid = make_grid(in);
//    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    auto start = grid.find_first('S');
    grid.set_value(start, 'a');
    auto end = grid.find_first('E');
    grid.set_value(end, 'z');

    auto graph = build_graph(grid);
    auto path = graph.dijkstra_with_path(
        (long)grid.underlying_idx(start),
        (long)grid.underlying_idx(end)
    );

    draw_path(grid, graph, path);

    // -1 because start is the zeroth step
    return fmt::format("{}", path.size() - 1);
}

SOLVER(2022, 12, 2, true)
(const std::string &in) {
    auto grid = make_grid(in);
//    auto grid = make_grid(std::string(EXAMPLE_INPUT_1));

    // Still need to set the start to a
    auto start = grid.find_first('S');
    grid.set_value(start, 'a');

    std::vector<GridCell> all_possible_starts;
    for (long row = 0; row < grid.rows; ++row) {
        for (long column = 0; column < grid.columns; ++column) {
            if (grid.at(column, row) == 'a') all_possible_starts.push_back(GridCell{row, column});
        }
    }

    auto end = grid.find_first('E');
    grid.set_value(end, 'z');

    auto graph = build_graph(grid);

    std::vector<std::vector<long>> paths;
    for (const auto &one_start: all_possible_starts) {
        auto path = graph.dijkstra_with_path(
                (long)grid.underlying_idx(one_start),
                (long)grid.underlying_idx(end)
        );
        if (path.size() == 1) continue; // This means that there is actually no path
        paths.push_back(path);
    }

    std::sort(paths.begin(), paths.end(), [](auto const &a, auto const &b) {
        return a.size() < b.size();
    });
    auto shortest_path = paths.at(0);

    draw_path(grid, graph, shortest_path);

    // -1 because start is the zeroth step
    return fmt::format("{}", shortest_path.size() - 1);
}
