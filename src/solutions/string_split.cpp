#include "./string_split.h"
#include <sstream>

std::vector<std::string> string_split(const std::string &in, const char& delimiter) {
    std::vector<std::string> tokens;
    std::istringstream stream(in);
    std::string token;

    while (std::getline(stream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

std::vector<std::string> string_split(const std::string &in, const std::string &delimiter) {
    std::vector<std::string> tokens;
    std::size_t start = 0;
    std::size_t end = in.find(delimiter);

    while (end != std::string::npos) {
        tokens.push_back(in.substr(start, end - start));
        start = end + 1;
        end = in.find(delimiter, start);
    }
    tokens.push_back(in.substr(start));

    return tokens;
}
