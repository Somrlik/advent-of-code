#include <numeric>
#include <regex>
#include "../one_solution.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
""
"abc"
"aaa\"aaa"
"\x27"
)";
#pragma endregion

SOLVER(2015, 8, 1, true)
(const std::string &in) {
    auto lines = string_split(trim(in), '\n');
//    auto lines = string_split(trim(std::string(EXAMPLE_INPUT_1)), '\n');

    long diff = 0;
    const std::vector<std::regex> regexes = {std::regex(R"(\\\\)"), std::regex(R"(\\")"), std::regex(R"(\\x[0-9a-f]{2})")};

    for (const auto& line: lines) {
        diff += (long)line.size();
        std::string out = line;
        for (const auto& regex: regexes) {
            out = std::regex_replace(out, regex, "#");
        }
        diff -= (long)out.size() - 2;
    }

    return fmt::format("{}", diff);
}

SOLVER(2015, 8, 2, false)
(const std::string &in) {
    auto lines = string_split(trim(in), '\n');
//    auto lines = string_split(trim(std::string(EXAMPLE_INPUT_1)), '\n');

    long diff = 0;
    const std::vector<std::tuple<std::regex, std::string>> regexes_replacements = {
            {std::regex(R"(")"), "1200"},
            {std::regex(R"(\\)"), "1200"},
            {std::regex(R"(\\x[0-9a-f]{2})"), "12345"},
    };

    for (const auto& line: lines) {
        diff += (long)line.size();
        std::string out = line;
        for (const auto& [regex, replacement]: regexes_replacements) {
            out = std::regex_replace(out, regex, replacement);
        }
        diff -= (long)out.size() - 2;
    }

    // should be 2074 using sed
    return fmt::format("{}", diff);
}
