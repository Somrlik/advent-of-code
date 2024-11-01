#include "../one_solution.h"
#include "../../trim.h"
#include "../grid.h"
#include "../string_split.h"
#include <regex>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
R 6 (#70c710)
D 5 (#0dc571)
L 2 (#5713f0)
D 2 (#d2c081)
R 2 (#59c680)
D 2 (#411b91)
L 5 (#8ceee2)
U 2 (#caa173)
L 1 (#1b58a2)
U 2 (#caa171)
R 2 (#7807d2)
U 3 (#a77fa3)
L 2 (#015232)
U 2 (#7a21e3)
)";

namespace {
    typedef enum Direction {
        UP,
        RIGHT,
        DOWN,
        LEFT,
    } Direction;

    typedef struct Instruction {
        Direction direction;
        long length;
        int color;
    } Instruction;
}

static std::map<char, Direction> direction_map = {
        {'U', UP},
        {'R', RIGHT},
        {'D', DOWN},
        {'L', LEFT},
};

static std::vector<Instruction> parse_instructions(const std::string &in) {
    auto instructions_str = string_split(trim(in), '\n');
    std::vector<Instruction> instructions;

    const std::regex regex(R"(([URDL])\s([0-9]+)\s\(#([0-9a-fA-F]{6})\))");
    for (auto const &instruction_str: instructions_str) {
        std::smatch raw_matches;

        std::regex_search(instruction_str, raw_matches, regex);
        instructions.push_back(Instruction{
            direction_map.at(raw_matches[1].str().at(0)),
            std::stol(raw_matches[2].str()),
            std::stoi(raw_matches[3].str(), nullptr, 16),
        });
    }

    return instructions;
}

namespace {
    struct Point {
        double x, y;

        Point operator+(const Point &other) const {
            return Point{x + other.x, y + other.y};
        }

        Point operator+(double addend) const {
            return Point{x + addend, y + addend};
        }

        Point operator-(const Point &other) const {
            return Point{x - other.x, y - other.y};
        }

        Point operator-(double subtrahend) const {
            return Point{x - subtrahend, y - subtrahend};
        }

        Point operator*(const Point &other) const {
            return Point{x - other.x, y - other.y};
        }

        Point operator*(double multiplier) const {
            return Point{x * multiplier, y * multiplier};
        }
    };

    struct dig_trench_return {
        std::vector<Point> polygon_points;
        std::vector<std::tuple<Point, Point, int>> colors;
    };
}

static std::map<Direction, Point> direction_to_vector = {
        {UP, {-1, 0}},
        {RIGHT, {0, +1}},
        {DOWN, {+1, 0}},
        {LEFT, {0, -1}},
};

static dig_trench_return dig_trench(const std::vector<Instruction>& instructions) {
    // Construct a polygon
    std::vector<Point> points;
    std::vector<std::tuple<Point, Point, int>> colors;

    Point current_point{0, 0};
    points.push_back({0, 0});
    for (const auto &instruction: instructions) {
        auto next_point = current_point + direction_to_vector.at(instruction.direction) * (double)instruction.length;
        colors.emplace_back(current_point, next_point, instruction.color);
        points.emplace_back(next_point);
        current_point = next_point;
    }

    return {points, colors};
}

static double get_trench_area(const std::vector<Point>& points) {
    int size = (int)points.size();
    auto area = 0.0;

    // Shoelace formula
    for (int line_idx = 0; line_idx < size; ++line_idx) {
        int j = (line_idx + 1) % size;  // Next vertex (wrapping around at the end)
        area += points[line_idx].x * points[j].y;
        area -= points[line_idx].y * points[j].x;
    }

    area = std::abs(area) / 2.0;
    return area;
}

SOLVER(2023, 18, 1, false)
(const std::string &in) {
//    auto instructions = parse_instructions(trim(in));
    auto instructions = parse_instructions(trim(std::string(EXAMPLE_INPUT_1)));

    auto [trench_poly, color] = dig_trench(instructions);
    auto area = get_trench_area(trench_poly);

    // 10988 too low
    return fmt::format("{}", area);
}

SOLVER(2023, 18, 2, false)
(const std::string &in) {
    return "n/a";
}
