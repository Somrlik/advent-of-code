#include "../one_solution.h"

#include <sstream>
#include <map>
#include <algorithm>
#include <numeric>

SOLVER(2015, 5, 1, true)
(const std::string &str) {
    int counter = 0;
    std::vector<std::string> forbidden = {"ab", "cd", "pq", "xy"};

    std::string line;
    std::istringstream stream(str);
    // std::istringstream stream("ugknbfddgicrmopn");
    // std::istringstream stream("aaa");
    // std::istringstream stream("jchzalrnumimnmhp");
    // std::istringstream stream("haegwjzuvuyypxyu");
    // std::istringstream stream("dvszwmarrgswjxmb");

    while (std::getline(stream, line)) {
        // First of all check for forbidden strings since it will be fast
        bool found_forbidden = false;
        for (auto const &to_find: forbidden) {
            std::size_t found = line.find(to_find);
            if (found != std::string::npos) {
                found_forbidden = true;
                break;
            }
        }

        if (found_forbidden) continue;

        std::map<char, int> vowel_counter = {
                {'a', 0},
                {'e', 0},
                {'i', 0},
                {'o', 0},
                {'u', 0},
        };

        bool has_duplicates = false;
        std::pair<char, char> duplicate_buffer = {'\0', '\1'};

        for (auto c : line) {
            if (vowel_counter.contains(c)) {
                vowel_counter.at(c) += 1;
            }

            if (has_duplicates) continue;
            duplicate_buffer.first = duplicate_buffer.second;
            duplicate_buffer.second = c;
            if (duplicate_buffer.first == duplicate_buffer.second) {
                has_duplicates = true;
            }
        }

        if (!has_duplicates) continue;
        auto sum = std::accumulate(vowel_counter.begin(), vowel_counter.end(), 0, [](const int previous, const auto& element) {
            return previous + element.second;
        });
        if (sum < 3) continue;
        counter++;
    }

    return fmt::format("{}", counter);
}

SOLVER(2015, 5, 2, false)
(const std::string &str) {
    int counter = 0;

    std::string line;
    std::istringstream stream(str);
    // Fails on this in particular
    // std::istringstream stream("rxexcbwhiywwwwnu");

    while (std::getline(stream, line)) {
        char triplet_buff[] = {'\0', '\1', '\2'};

        std::map<int, int> pair_counter = {};
        bool has_duplicate_pair = false;
        bool has_gap_repeats = false;

        for (auto c : line) {
            // Rotate buffer
            triplet_buff[0] = triplet_buff[1];
            triplet_buff[1] = triplet_buff[2];
            triplet_buff[2] = c;

            if (!has_duplicate_pair) {
                // Must check second and third
                unsigned int pair_hash = ((int)triplet_buff[1] << 8) | ((int)triplet_buff[2]);

                if (pair_counter.find(pair_hash) != pair_counter.end()) {
                    // fmt::println("found pair {}{}", triplet_buff[1], triplet_buff[2]);
                    // Check for overlaps, will only ever occur with 3 identical chars
                    if (triplet_buff[1] != triplet_buff[2] || triplet_buff[0] != triplet_buff[1]) {
                        has_duplicate_pair = true;
                    }
                } else {
                    pair_counter.insert(std::pair<int, int> {pair_hash, 0});
                }
            }

            if (triplet_buff[0] == triplet_buff[2]) {
                has_gap_repeats = true;
            }
        }

        fmt::println("{} gap: {} pairs: {}", line, has_gap_repeats, has_duplicate_pair);
        // If both are true, then let's count it
        if (has_duplicate_pair && has_gap_repeats) {
            counter++;
        }
    }

    // The solution should be 51, it's 50
    return fmt::format("{}", counter);
}
