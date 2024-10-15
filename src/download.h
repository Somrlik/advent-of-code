#ifndef AOC_DOWNLOAD_H
#define AOC_DOWNLOAD_H

#include <string>

#ifdef USE_CURL
constexpr const bool CAN_DOWNLOAD = true;
#else
constexpr const bool CAN_DOWNLOAD = false;
#endif

typedef struct download_return {
    bool success;
    bool bad_key;
    std::string contents;
    std::string error;
} download_return;

download_return download_puzzle_input(uint year, uint day, const std::string& key);

#endif
