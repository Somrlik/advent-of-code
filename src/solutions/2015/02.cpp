#include "../one_solution.h"

#include <regex>
#include <numeric>

SOLVER(2015, 2, 1, true)
(const std::string &in) {
    long sum = 0;

    std::string line;
    std::istringstream stream(in);

    while (std::getline(stream, line)) {
        std::regex rgx("([0-9]+)x([0-9]+)x([0-9]+)");
        std::smatch raw_matches;
        std::vector<int> matches = {0, 0, 0}; // l, w, h

        std::regex_search(line, raw_matches, rgx);
        for (size_t i = 1; i < raw_matches.size(); ++i) {
            matches.at(i - 1) = std::stoi(raw_matches[i].str());
        }

        std::vector<int> sides = {
                2 * matches.at(0) * matches.at(1),
                2 * matches.at(1) * matches.at(2),
                2 * matches.at(2) * matches.at(0),
        };

        auto min = *std::min_element(sides.begin(), sides.end()) / 2;
        int one_sum = std::accumulate(sides.begin(), sides.end(), 0) + min;
        sum += one_sum;
    }

    return fmt::format("{}", sum);
}

std::pair<int, int> smallest_and_second_smallest(std::vector<int>& numbers) {
    int smallest = std::numeric_limits<int>::max();
    int second_smallest = std::numeric_limits<int>::max();

    for (int num : numbers) {
        if (num < smallest) {
            second_smallest = smallest;
            smallest = num;
        } else if (num <= second_smallest) {
            second_smallest = num;
        }
    }

    return std::make_pair(smallest, second_smallest);
}

SOLVER(2015, 2, 2, true)
(const std::string &in) {
    long sum = 0;

    std::string line;
    std::istringstream stream(in);
    // std::istringstream stream("1x1x10");
    // std::istringstream stream("2x3x4");

    while (std::getline(stream, line)) {
        std::regex rgx("([0-9]+)x([0-9]+)x([0-9]+)");
        std::smatch raw_matches;
        std::vector<int> matches = {0, 0, 0}; // l, w, h

        std::regex_search(line, raw_matches, rgx);
        for (size_t i = 1; i < raw_matches.size(); ++i) {
            matches.at(i - 1) = std::stoi(raw_matches[i].str());
        }

        auto smallest = smallest_and_second_smallest(matches);
        int one_sum = std::accumulate(matches.begin(), matches.end(), 1, std::multiplies<>());
        one_sum += smallest.first * 2 + smallest.second * 2;
        sum += one_sum;
    }

    return fmt::format("{}", sum);
}
