#include "../one_solution.h"
#include "../string_split.hpp"
#include "../../trim.h"

#include <sstream>
#include <map>

constexpr const uint RED_CUBES_TOTAL = 12;
constexpr const uint GREEN_CUBES_TOTAL = 13;
constexpr const uint BLUE_CUBES_TOTAL = 14;

typedef enum Color {
    RED,
    GREEN,
    BLUE
} Color;

const std::map<std::string, Color> color_str_to_color = {
        {"red", RED},
        {"green", GREEN},
        {"blue", BLUE}
};

typedef struct Game {
    uint id;
    std::vector<std::map<Color, uint>> pulls;
} Game;

std::vector<Game> parse_games(const std::string &in) {
    std::istringstream stream(trim(in));
    std::string line;
    std::vector<Game> games;

    while (std::getline(stream, line)) {
        auto game_id_and_pulls = string_split(line, ':');
        if (game_id_and_pulls.size() != 2) {
            throw std::logic_error("The split must only contain the game id and steps");
        }

        auto game_id_str = game_id_and_pulls.at(0).erase(0, strnlen("Game ", 5));
        uint game_id = std::stoul(game_id_str);

        auto pulls_str = string_split(game_id_and_pulls.at(1), ';');
        std::vector<std::map<Color, uint>> pulls;

        for (const auto &pull_str: pulls_str) {
            auto colors_str = string_split(trim(pull_str), ',');
            std::map<Color, uint> colors;

            for (const auto &color_str: colors_str) {
                auto count_color_str = string_split(trim(color_str), ' ');
                if (count_color_str.size() != 2) {
                    throw std::logic_error("The split must only contain count and color");
                }

                uint count = std::stoul(count_color_str.at(0));
                Color color = color_str_to_color.at(count_color_str.at(1));
                colors.insert({color, count});
            }
            pulls.push_back(colors);
        }

        games.push_back({game_id, pulls});
    }

    return games;
}

constexpr const std::string_view EXAMPLE_1 = R"(
Game 1: 3 blue, 4 red; 1 red, 2 green, 6 blue; 2 green
Game 2: 1 blue, 2 green; 3 green, 4 blue, 1 red; 1 green, 1 blue
Game 3: 8 green, 6 blue, 20 red; 5 blue, 4 red, 13 green; 5 green, 1 red
Game 4: 1 green, 3 red, 6 blue; 3 green, 6 red; 3 green, 15 blue, 14 red
Game 5: 6 red, 1 blue, 3 green; 2 blue, 1 red, 2 green
)";

SOLVER(2023, 2, 1, true)
(const std::string &in) {
//    auto games = parse_games(std::string(EXAMPLE_1));
    auto games = parse_games(in);

    ulong id_sum = 0;
    for (const auto& game: games) {
        bool game_possible = true;
        for (const auto& pull: game.pulls) {
            for (const auto& [color, count]: pull) {
                if (
                        (color == RED && count > RED_CUBES_TOTAL) ||
                        (color == GREEN && count > GREEN_CUBES_TOTAL) ||
                        (color == BLUE && count > BLUE_CUBES_TOTAL)
                    )
                {
                    game_possible = false;
                    goto game_loop_break;
                }
            }
        }

        game_loop_break:
        if (game_possible) {
            id_sum += game.id;
        }
    }

    return fmt::format("{}", id_sum);
}

SOLVER(2023, 2, 2, true)
(const std::string &in) {
//        auto games = parse_games(std::string(EXAMPLE_1));
    auto games = parse_games(in);

    ulong cubes_sum = 0;
    for (const auto& game: games) {
        std::map<Color, uint> min_cubes = {
                {RED, 0},
                {GREEN, 0},
                {BLUE, 0},
        };

        for (const auto& pull: game.pulls) {
            for (const auto& [color, count]: pull) {
                min_cubes.insert_or_assign(color, std::max(min_cubes.at(color), count));
            }
        }

        cubes_sum += (min_cubes.at(RED) * min_cubes.at(GREEN) * min_cubes.at(BLUE));
    }

    return fmt::format("{}", cubes_sum);
}
