#include <map>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
A Y
B X
C Z
)";

namespace {
    typedef enum Hand {
        ROCK = 1,
        PAPER = 2,
        SCISSORS = 3,
    } Hand;

    typedef enum Outcome {
        LOSS = 0,
        DRAW = 3,
        WIN = 6,
    } Outcome;
}

static std::map<std::pair<Hand, Hand>, Outcome> game_states = {
        {{ROCK, ROCK}, DRAW},
        {{ROCK, PAPER}, LOSS},
        {{ROCK, SCISSORS}, WIN},

        {{PAPER, ROCK}, WIN},
        {{PAPER, PAPER}, DRAW},
        {{PAPER, SCISSORS}, LOSS},

        {{SCISSORS, ROCK}, LOSS},
        {{SCISSORS, PAPER}, WIN},
        {{SCISSORS, SCISSORS}, DRAW},
};

static std::map<char, Hand> opponent_plays = {
        {'A', ROCK},
        {'B', PAPER},
        {'C', SCISSORS},
};

static std::map<char, Hand> my_plays = {
        {'X', ROCK},
        {'Y', PAPER},
        {'Z', SCISSORS},
};

static std::vector<std::pair<Hand, Hand>> parse_rounds(const std::string& in) {
    std::vector<std::pair<Hand, Hand>> plays;

    auto plays_str = string_split(trim(in), '\n');
    for (auto const &play_str: plays_str) {
        plays.emplace_back(my_plays.at(play_str.at(2)),opponent_plays.at(play_str.at(0)));
    }

    return plays;
}

SOLVER(2022, 2, 1, true)
(const std::string &in) {
//    auto rounds = parse_rounds(std::string(EXAMPLE_INPUT_1));
    auto rounds = parse_rounds(in);

    long score = 0;
    for (const auto &[my_hand, opponent_hand]: rounds) {
        auto outcome = game_states.at({my_hand, opponent_hand});
        score += (long)my_hand + (long)outcome;
    }

    return fmt::format("{}", score);
}

static std::map<std::pair<Hand, Outcome>, Hand> strategy_states = {
        {{ROCK, DRAW}, ROCK},
        {{ROCK, WIN}, PAPER},
        {{ROCK, LOSS}, SCISSORS},

        {{PAPER, LOSS}, ROCK},
        {{PAPER, DRAW}, PAPER},
        {{PAPER, WIN}, SCISSORS},

        {{SCISSORS, WIN}, ROCK},
        {{SCISSORS, LOSS}, PAPER},
        {{SCISSORS, DRAW}, SCISSORS},
};

static std::map<char, Outcome> my_strategy = {
        {'X', LOSS},
        {'Y', DRAW},
        {'Z', WIN},
};

static std::vector<std::pair<Hand, Outcome>> parse_rounds_strategy(const std::string& in) {
    std::vector<std::pair<Hand, Outcome>> plays;

    auto plays_str = string_split(trim(in), '\n');
    for (auto const &play_str: plays_str) {
        plays.emplace_back(opponent_plays.at(play_str.at(0)), my_strategy.at(play_str.at(2)));
    }

    return plays;
}

SOLVER(2022, 2, 2, true)
(const std::string &in) {
//        auto rounds = parse_rounds_strategy(std::string(EXAMPLE_INPUT_1));
    auto rounds = parse_rounds_strategy(in);

    long score = 0;
    for (const auto &[opponent_hand, outcome]: rounds) {
        auto my_hand = strategy_states.at({opponent_hand, outcome});
        score += (long)my_hand + (long)outcome;
    }

    return fmt::format("{}", score);
}
