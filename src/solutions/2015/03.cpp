#include "../one_solution.h"

static auto pair_compare = [](std::pair<int, int> a, std::pair<int, int> b) {
    return a.first < b.first || (!(b.first < a.first) && a.second < b.second);
};

SOLVER(2015, 3, 1, true)
(const std::string &str) {
    std::set<std::pair<int, int>, decltype(pair_compare)> visited(pair_compare);

    std::pair<int, int> position = {0, 0};
    visited.insert(position);

    // std::string example = ">"; // 2
    // std::string example = "^>v<"; // 4
    // std::string example = "^v^v^v^v^v"; // 2

    for (auto c : str) {
        if (c == '^') {
            position.first++;
        } else if (c == 'v') {
            position.first--;
        } else if (c == '>') {
            position.second++;
        } else if (c == '<') {
            position.second--;
        }

        visited.insert(position);
    }

    return fmt::format("{}", visited.size());
}

SOLVER(2015, 3, 2, true)
(const std::string &str) {
    std::set<std::pair<int, int>, decltype(pair_compare)> visited(pair_compare);

    std::pair<int, int> first_position = {0, 0};
    std::pair<int, int> second_position = {0, 0};
    visited.insert(first_position);
    int idx = 0;

    // std::string example = "^v"; // 3
    // std::string example = "^>v<"; // 3
    // std::string example = "^v^v^v^v^v"; // 11

    for (auto c : str) {
        auto active_position = idx % 2 == 0 ? &first_position : &second_position;

        if (c == '^') {
            active_position->first++;
        } else if (c == 'v') {
            active_position->first--;
        } else if (c == '>') {
            active_position->second++;
        } else if (c == '<') {
            active_position->second--;
        }

        visited.insert(first_position);
        visited.insert(second_position);
        idx++;
    }

    return fmt::format("{}", visited.size());
}