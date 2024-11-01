#include "../one_solution.h"
#include "../grid.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
.|...\....
|.-.\.....
.....|-...
........|.
..........
.........\
..../.\\..
.-.-/..|..
.|....-|.\
..//.|....
)";

constexpr const std::string_view CRAFTED_LOOP = R"(
.|.\
....
.-./
)";

constexpr const std::string_view CRAFTED_BIDI = R"(
.|./.\
......
..././
.-./..
)";

namespace {
    typedef enum Direction {
        NORTH = 0,
        EAST = 1,
        SOUTH = 2,
        WEST = 3,
    } Direction;

    typedef struct Beam {
        Direction entering_from;
        GridCell current_position;
        bool journey_complete = false;
        std::string label;
    } Beam;

    typedef struct VisitedPosition {
        Direction from;
        GridCell position;

        bool operator<(const VisitedPosition& other) const {
            return (this->from == other.from) ? this->position < other.position : this->from < other.from;
        }

        bool operator==(const VisitedPosition& other) const {
            return (this->from == other.from) && (this->position == other.position);
        }
    } VisitedPosition;
}

static std::vector<Beam> tick_beam(const Grid& grid, Beam& beam, std::set<VisitedPosition>& visited_positions) {
    if (beam.journey_complete) return {};

    auto encounter = grid.at(beam.current_position);
    bool passing_thru = false;
    GridCell next_position_vector;
    std::vector<Beam> new_beams;

    if (visited_positions.contains({beam.entering_from, beam.current_position})) {
        beam.journey_complete = true;
        return {};
    } else {
        visited_positions.insert({beam.entering_from, beam.current_position});
    }

    if (encounter == '\0') {
        beam.journey_complete = true;
        return {};
    } else if (encounter == '/') {
        if (beam.entering_from == NORTH) {
            // Beam goes west
            next_position_vector = {0, -1};
            beam.entering_from = EAST;
        } else if (beam.entering_from == EAST) {
            // Beam goes south
            next_position_vector = {+1, 0};
            beam.entering_from = NORTH;
        } else if (beam.entering_from == SOUTH) {
            // Beam goes east
            next_position_vector = {0, +1};
            beam.entering_from = WEST;
        } else if (beam.entering_from == WEST) {
            // Beam goes north
            next_position_vector = {-1, 0};
            beam.entering_from = SOUTH;
        }
    } else if (encounter == '\\') {
        if (beam.entering_from == NORTH) {
            // Beam goes east
            next_position_vector = {0, +1};
            beam.entering_from = WEST;
        } else if (beam.entering_from == EAST) {
            // Beam goes north
            next_position_vector = {-1, 0};
            beam.entering_from = SOUTH;
        } else if (beam.entering_from == SOUTH) {
            // Beam goes west
            next_position_vector = {0, -1};
            beam.entering_from = EAST;
        } else if (beam.entering_from == WEST) {
            // Beam goes south
            next_position_vector = {+1, 0};
            beam.entering_from = NORTH;
        }
    } else if (encounter == '|') {
        if (beam.entering_from == EAST || beam.entering_from == WEST) {
            // Beam splits into two - north and south
            // current one heads south
            next_position_vector = {+1, 0};
            beam.entering_from = NORTH;

            // new one is created and heads north
            new_beams = {
                Beam{SOUTH, {beam.current_position.row - 1, beam.current_position.column}},
            };
        } else {
            // Else beam passes thru
            passing_thru = true;
        }
    } else if (encounter == '-') {
        if (beam.entering_from == NORTH || beam.entering_from == SOUTH) {
            // Beam splits into two - east and west

            // current one heads west
            next_position_vector = {0, -1};
            beam.entering_from = EAST;

            // new one is created and heads east
            new_beams = {
                Beam{WEST, {beam.current_position.row, beam.current_position.column + 1}},
            };
        } else {
            // Else beam passes thru
            passing_thru = true;
        }
    } else if (encounter == '.') {
        // Beam passes thru
        passing_thru = true;
    }

    if (passing_thru) {
        if (beam.entering_from == NORTH) {
            next_position_vector = {+1, 0};
        } else if (beam.entering_from == EAST) {
            next_position_vector = {0, -1};
        } else if (beam.entering_from == SOUTH) {
            next_position_vector = {-1, 0};
        } else if (beam.entering_from == WEST) {
            next_position_vector = {0, +1};
        }
    }

    if (!beam.journey_complete) {
        beam.current_position += next_position_vector;
        if (!grid.contains(beam.current_position)) {
            beam.journey_complete = true;
        }
    }

    if (!new_beams.empty()) {
        new_beams.at(0).label = beam.label + fmt::format("->[{},{}]", new_beams.at(0).current_position.row, new_beams.at(0).current_position.column);
    }

    return new_beams;
}

static long determine_energy(const Grid &grid, const Direction &from, const GridCell &start) {
    std::vector<Beam> beams;
    beams.emplace_back(Beam{from, start});
    beams.at(0).label = "[0,0]";

    std::set<VisitedPosition> visited_positions;

    long counter = 0;
    while (true) {
        counter++;
        std::vector<Beam> beams_to_add;
        for (auto &beam: beams) {
            if (beam.journey_complete) continue;
            auto new_beams = tick_beam(grid, beam, visited_positions);
            for (const auto& new_beam: new_beams) {
                if (grid.contains(new_beam.current_position)) beams_to_add.push_back(new_beam);
            }
        }

        beams.insert(beams.end(), beams_to_add.begin(), beams_to_add.end());

        // If all journeys have been completed, exit loop
        if (std::all_of(beams.begin(), beams.end(), [](auto const &beam) {
            return beam.journey_complete;
        })) {
            break;
        }
    }

    std::set<GridCell> energized_cells;
    for (const auto &[_, visited]: visited_positions) {
        energized_cells.insert(visited);
    }

    return (long)energized_cells.size();
}

SOLVER(2023, 16, 1, true)
(const std::string &in) {
    auto grid = make_grid(trim(in));
//    auto grid = make_grid(trim(std::string(EXAMPLE_INPUT_1)));
//    auto grid = make_grid(trim(std::string(CRAFTED_LOOP)));
//    auto grid = make_grid(trim(std::string(CRAFTED_BIDI)));

    return fmt::format("{}", determine_energy(grid, WEST, {0, 0}));
}

SOLVER(2023, 16, 2, true)
(const std::string &in) {
    auto grid = make_grid(trim(in));
//    auto grid = make_grid(trim(std::string(EXAMPLE_INPUT_1)));

    long max_energy = std::numeric_limits<long>::min();
    // From north, south
    for (long column = 0; column < grid.columns; ++column) {
        // north
        long energy_north = determine_energy(grid, NORTH, GridCell{0, column});
        fmt::println("From NORTH pos {},{} energy={}", 0, column, energy_north);
        max_energy = std::max(max_energy, energy_north);

        // south
        long energy_south = determine_energy(grid, SOUTH, GridCell{(long)(grid.rows - 1), column});
        fmt::println("From SOUTH pos {},{} energy={}", grid.rows - 1, column, energy_south);
        max_energy = std::max(max_energy, energy_south);
    }

    // From east, west
    for (long row = 0; row < grid.rows; ++row) {
        // east
        long energy_east = determine_energy(grid, EAST, GridCell{row, (long)(grid.columns - 1)});
        fmt::println("From EAST pos {},{} energy={}", row, (long)(grid.columns - 1), energy_east);
        max_energy = std::max(max_energy, energy_east);

        // west
        long energy_west = determine_energy(grid, WEST, GridCell{row, 0});
        fmt::println("From WEST pos {},{} energy={}", row, 0, energy_west);
        max_energy = std::max(max_energy, energy_west);
    }

    return fmt::format("{}", max_energy);
}
