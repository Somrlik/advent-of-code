#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"
#include <numeric>
#include <regex>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
LR

11A = (11B, XXX)
11B = (XXX, 11Z)
11Z = (11B, XXX)
22A = (22B, XXX)
22B = (22C, 22C)
22C = (22Z, 22Z)
22Z = (22B, 22B)
XXX = (XXX, XXX)
)";

template <typename N>
N lcm_vector(std::vector<N> numbers) {
    if (numbers.size() < 2) {
        throw std::logic_error("At least two numbers are expected to find LCM.");
    }

    return std::accumulate(numbers.begin(), numbers.end(), numbers.at(0), [](N acc, N num) {
        return std::lcm(acc, num);
    });
}

typedef enum Direction {
    DIRECTION_LEFT = 0,
    DIRECTION_RIGHT = 1,
} Direction;

typedef struct MapNode {
    std::string src;
    std::string left;
    std::string right;
} MapNode;

struct parse_return {
    std::vector<Direction> directions;
    std::map<std::string, MapNode> map_nodes;
};

static parse_return parse_input(const std::string &in) {
    const auto directions_and_map_str = string_split(trim(in), "\n\n");

    std::vector<Direction> directions(directions_and_map_str.at(0).length());
    std::transform(directions_and_map_str.at(0).begin(), directions_and_map_str.at(0).end(), directions.begin(), [](char c) {
        return c == 'L' ? DIRECTION_LEFT : DIRECTION_RIGHT;
    });

    std::regex pattern(R"(([0-9A-Z]+) = \(([0-9A-Z]+), ([0-9A-Z]+)\))");

    auto map_str = string_split(trim(directions_and_map_str.at(1)), '\n');
    std::map<std::string, MapNode> nodes;
    std::transform(map_str.begin(), map_str.end(), std::inserter(nodes, nodes.begin()), [&pattern](const auto& line) {
        std::smatch matches;

        if (std::regex_search(line, matches, pattern)) {
            std::string src = matches[1];
            std::string left = matches[2];
            std::string right = matches[3];

            return std::pair<std::string, MapNode>{src, {src, left, right}};
        }

        throw std::logic_error("There must be a regex match in the line");
    });

    return {directions, nodes};
}

SOLVER(2023, 8, 1, true)
(const std::string &in) {
    auto [directions, map_nodes] = parse_input(in);
//    auto [directions, map_nodes] = parse_input(std::string(EXAMPLE_INPUT_1));

    std::string current = "AAA";
    ulong steps = 0;
    ulong direction_idx = 0;
    do {
        if (direction_idx >= directions.size()) direction_idx = 0;
        auto direction = directions[direction_idx];
        current = (direction == DIRECTION_LEFT) ? map_nodes[current].left : map_nodes[current].right;
        steps++;
        direction_idx++;
    } while (current != "ZZZ");

    return fmt::format("{}", steps);
}

SOLVER(2023, 8, 2, true)
(const std::string &in) {
    auto [directions, map_nodes] = parse_input(in);
//    auto [directions, map_nodes] = parse_input(std::string(EXAMPLE_INPUT_1));

    std::vector<ulong> periods;
    std::vector<std::string> currents;
    std::for_each(map_nodes.begin(), map_nodes.end(), [&currents](const auto& pair) {
        if (pair.first[2] == 'A') currents.push_back(pair.first);
    });

    ulong step = 0;
    ulong direction_idx = 0;
    do {
        if (direction_idx >= directions.size()) direction_idx = 0;
        auto direction = directions[direction_idx];
        std::vector<std::string> nexts(currents.size());
        std::transform(currents.begin(), currents.end(), nexts.begin(), [&direction, &map_nodes](const auto &current) {
            return (direction == DIRECTION_LEFT) ? map_nodes[current].left : map_nodes[current].right;
        });

        currents = nexts;

        currents.erase(std::remove_if(currents.begin(), currents.end(), [&periods, &step](const auto& current) {
            if (current[2] == 'Z') {
                periods.push_back(step + 1);
                return true;
            }
            return false;
        }), currents.end());

        step++;
        direction_idx++;
    } while (!currents.empty());

    ulong lcm_of_periods = lcm_vector(periods);
    return fmt::format("{}", lcm_of_periods);
}
