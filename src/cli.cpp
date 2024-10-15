#include "../extern/cxxopts.hpp"
#include <fmt/core.h>

// Start up fmtlog
#include "../extern/fmtlog/fmtlog-inl.h"
#include "storage.h"
#include "download.h"
#include "solutions.h"

#include <iostream>
#include <fstream>
#include <filesystem>

constexpr const uint YEAR_FROM = 2015;
constexpr const uint DAY_FROM = 1;
constexpr const uint DAY_TO = 25;

static bool is_valid_puzzle_number(uint year, uint day) {
    if (YEAR_FROM >= year) {
        return false;
    }

    if (!(DAY_FROM <= day && day <= DAY_TO)) {
        return false;
    }

    return true;
}

static std::string ask_for_key() {
    std::string key;
    fmt::println("Please visit https://adventofcode.com, log in and enter");
    fmt::println("the value of cookie named \"session\" below.");
    fmt::println("Your key will be saved in your local profile storage, you can use \"--delete-profile\"");
    fmt::println("to delete it afterwards.");
    std::cout << "key: ";
    std::cin >> key;
    return key;
}

int main(int argc, char* argv[]) {
    // fmtlog::startPollingThread();

    cxxopts::Options options("aoc", R"HELP(
CLI utility for adventofcode.com

Allows you to easily fetch inputs and implement solutions
for puzzles from adventofcode.com.
)HELP");

    options.add_options()
            ("year", "The year", cxxopts::value<uint>()->default_value("2015"))
            ("day", "The day", cxxopts::value<uint>()->default_value("1"))
            ("level", "The level", cxxopts::value<uint>()->default_value("1"))
            ("f,file", "Puzzle input file", cxxopts::value<std::string>()->default_value(""))
            ("delete-profile", "Deletes current profile")
            ("d,debug", "Prints out debugging messages")
            ("q,quiet", "Suppresses errors and warnings")
            ("h,help", "Print usage");

    options.positional_help("[year] [day] [level] Allows you to specify the puzzle");

    try {
        options.parse_positional({"year", "day", "level"});
        auto opts = options.parse(argc, argv);

        bool quiet = opts.count("quiet");
        // TODO: Disable fmt::println when quiet

        // Turn of all logging when quiet, all messages when debug and WRN and ERR by default
        if (quiet) {
            fmtlog::setLogLevel(fmtlog::OFF);
        } else if (opts.count("debug")) {
            fmtlog::setLogLevel(fmtlog::DBG);
            logd("debug option specified, printing debug messages");
        } else {
            fmtlog::setLogLevel(fmtlog::WRN);
        }

        if (opts.count("help")) {
            logd("help option specified, printing help and exiting");
            fmt::println("{}", options.help());
            return 0;
        }

        if (opts.count("delete-profile")) {
            logd("profile deletion specified");
            if (!empty_storage()) {
                fmt::println("Failed to delete profile. Consider re-running with -d option.");
                return 1;
            }

            fmt::println("Profile deleted successfully.");
            return 0;
        }

        // Determine day, year, level
        uint year = opts["year"].as<uint>(), day = opts["day"].as<uint>(), level = opts["level"].as<uint>();
        logd("year: {}, day: {}, level: {}", year, day, level);
        if (!is_valid_puzzle_number(year, day)) {
            fmt::println("error: Year or day is not valid - AOC started at 2015, days are numbered from 1 till 25");
            return 1;
        }

        std::string puzzle_input;
        if (!opts.count("file")) {
            // Read from network
            logd("file not specified, reading storage");
            if (!initialize_storage()) {
                fmt::println("error: Failed to initialize permanent storage");
                return 3;
            }

            if (has_puzzle_input(year, day)) {
                logd("found cached puzzle input for {} {}", year, day);
                puzzle_input = get_puzzle_input(year, day);
                goto has_input;
            }

            logd("storage empty, trying network");

            if (!CAN_DOWNLOAD) {
                logw("cannot download anything, curl not linked");
                fmt::println("error: You must enable linking with CURL for download functionality");
                fmt::println("You will need to specify -DUSE_CURL when compiling.");
                return 1;
            }

            std::string key;
            if (!has_profile_key()) {
                logi("key was not found, asking for it");
                fmt::println("Your API key (cookie) was not found.");
                key = ask_for_key();
            } else {
                key = get_profile_key();
            }

            download_return result = download_puzzle_input(year, day, key);

            bool is_key_valid = false;
            while (!is_key_valid) {
                if (result.bad_key) {
                    fmt::println("error: Unable to log in with error: {}", result.error);
                    fmt::println("Please try to enter your key again or CTRL+C for exit.\n");
                    key = ask_for_key();
                    continue;
                }

                if (result.success) {
                    logd("key was good, storing");
                    store_key(key);
                    store_puzzle_input(year, day, result.contents);
                    puzzle_input = result.contents;
                    is_key_valid = true;
                } else {
                    loge("key was not bad, but there was no success? this is most likely a bug");
                    fmt::println("error: Unable to verify key with error {}", result.error);
                    return 2;
                }
            }
        } else {
            // Read file from filesystem
            auto infile = opts["file"].as<std::string>();
            logd("file specified: '{}'", infile);
            if (!std::filesystem::exists(infile)) {
                logd("'{}' does not exist, exiting", infile);
                fmt::println("error: File {} does not exist", infile);
                return 1;
            }

            try {
                std::ifstream in(infile, std::ios::in);
                in.seekg(0, std::ios::end);
                puzzle_input.resize(in.tellg());
                in.seekg(0, std::ios::beg);
                in.read(&puzzle_input[0], (long)puzzle_input.size());
                in.close();
            } catch (std::exception &e) {
                logd("failed to read '{}' with '{}', exiting", infile, e.what());
                fmt::println("error: Failed to read file {} because: ", infile, e.what());
                return 1;
            }

            logd("read {} bytes from '{}'", puzzle_input.size(), infile);
        }

        has_input:
        logd("got input by whatever means, it is {} chars long", puzzle_input.length());

        if (!has_solver(year, day, level)) {
            loge("could not find solver for {}/{}/{}", year, day, level);
            fmt::println("Failed to find solver for {}/{}/{}", year, day, level);
            return 1;
        }

        if (!is_solved(year, day, level)) {
            fmt::println("! This puzzle is NOT solved !");
        }

        std::string solution = run_solver(year, day, level, puzzle_input);

        fmt::println("The solution is: '{}'", solution);

        if (!is_solved(year, day, level)) {
            fmt::println("! This puzzle is NOT solved !");
        }
    } catch (cxxopts::exceptions::exception &e) {
        fmt::println("error: Failed to parse options: {}", e.what());
        fmt::println("Use --help for usage");
        return 1;
    }

    logd("finished");
    return 0;
}