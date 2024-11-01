#include "../one_solution.h"
#include "../../trim.h"
#include <map>

static long detect_start_of_packet(const std::string &in, int unique_marker_count) {
    auto const stream = trim(in);

    auto iterator = stream.begin();
    for (;;++iterator) {
        std::set<char> character_set(iterator, iterator + unique_marker_count);
        if (character_set.size() == unique_marker_count) {
            return (iterator + unique_marker_count) - stream.begin();
        }
    }
}

static const std::vector<std::tuple<std::string, long>> tests = {
        {"mjqjpqmgbljsphdztnvjfqwrcgsmlb", 7},
        {"bvwbjplbgvbhsrlpgdmjqwftvncz", 5},
        {"nppdvjthqldpwncqszvftbrmjlhg", 6},
        {"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 10},
        {"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 11},
};

SOLVER(2022, 6, 1, true)
(const std::string &in) {
    for (const auto& [str, result]: tests) {
        if (detect_start_of_packet(str, 4) != result) fmt::println("Wrong result for {}", str);
    }
    return fmt::format("{}", detect_start_of_packet(in, 4));
}

static const std::vector<std::tuple<std::string, long>> tests_14 = {
        {"mjqjpqmgbljsphdztnvjfqwrcgsmlb", 19},
        {"bvwbjplbgvbhsrlpgdmjqwftvncz", 23},
        {"nppdvjthqldpwncqszvftbrmjlhg", 23},
        {"nznrnfrfntjfmvfwmzdfjlvtqnbhcprsg", 29},
        {"zcfzfwzzqfrljwzlrfnpqdbhtmscgvjw", 26},
};

SOLVER(2022, 6, 2, true)
(const std::string &in) {
    for (const auto& [str, result]: tests_14) {
        if (detect_start_of_packet(str, 14) != result) fmt::println("Wrong result for {}", str);
    }
    return fmt::format("{}", detect_start_of_packet(in, 14));
}
