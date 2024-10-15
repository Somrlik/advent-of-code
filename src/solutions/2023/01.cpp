#include "../one_solution.h"

#include <sstream>
#include <cctype>
#include "../../trim.h"

SOLVER(2023, 1, 1, true)
(const std::string &in) {
    long sum = 0;

    std::string line;
    std::istringstream stream(in);

    while (std::getline(stream, line)) {
        char first = '\0';
        char second = '\0';

        for (const char &c: line) {
            if (std::isdigit(c)) {
                if (first == '\0') {
                    first = c;
                    second = c;
                } else {
                    second = c;
                }
            }
        }

        auto str_val = fmt::format("{}{}", first, second);
        sum += std::stol(str_val);
    }

    return fmt::format("{}", sum);
}

#include <map>
#include <vector>

std::map<std::string, char> words_to_long = {
    {"one", '1'},
    {"two", '2'},
    {"three", '3'},
    {"four", '4'},
    {"five", '5'},
    {"six", '6'},
    {"seven", '7'},
    {"eight", '8'},
    {"nine", '9'},
};

constexpr const std::size_t BUFFER_LEN = 100;

static std::string solve_lvl_2(const std::string&);
ADD_SOLUTION(2023, 1, 2, solve_lvl_2, true);

static std::string solve_lvl_2(const std::string &in) {
    long sum = 0;

//    std::string in = trim(R"(
//two1nine
//eightwothree
//abcone2threexyz
//xtwone3four
//4nineeightseven2
//zoneight234
//7pqrstsixteen
//)");

    std::string line;
    std::string buffer;
    buffer.reserve(BUFFER_LEN);
    std::istringstream stream(in);

    std::vector<std::string> words;
    words.reserve(words_to_long.size());
    for (const auto &[word, _]: words_to_long) {
        words.emplace_back(word);
    }

    while (std::getline(stream, line)) {
        char first = '\0';
        char second = '\0';

        for (const char &c: line) {
            if (std::isdigit(c)) {
                if (first == '\0') {
                    first = c;
                    second = c;
                } else {
                    second = c;
                }

                buffer.clear();
            } else {
                buffer.push_back(c);

                // Try to find the words in the buffer
                for (const auto& word: words) {
                    std::size_t pos = buffer.find(word);
                    if (pos != std::string::npos) {
                        // If found, strip before starting position
                        buffer.erase(0, pos);

                        auto found = words_to_long.find(buffer);
                        if (found != words_to_long.end()) {
                            if (first == '\0') {
                                first = found->second;
                                second = found->second;
                            } else {
                                second = found->second;
                            }
                            // Move the buffer only by one char so that "eightwo" is properly processed
                            buffer.erase(0, 1);
                        } else {
                            fmt::println("buffer '{}' does not contain any words...?", buffer);
                            buffer.clear();
                        }
                    }
                }
            }
        }
        buffer.clear();

        auto str_val = fmt::format("{}{}", first, second);
        sum += std::stol(str_val);
    }

    return fmt::format("{}", sum);
}
