#include <map>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"
#include <numeric>

constexpr const std::string_view EXAMPLE_1 = R"(
32T3K 765
T55J5 684
KK677 28
KTJJT 220
QQQJA 483
)";

typedef ulong CardStrength;

const std::map<char, CardStrength> STRENGTHS = {
        {'A', 14},
        {'K', 13},
        {'Q', 12},
        {'J', 11},
        {'T', 10},
        {'9', 9},
        {'8', 8},
        {'7', 7},
        {'6', 6},
        {'5', 5},
        {'4', 4},
        {'3', 3},
        {'2', 2},
};

const std::map<char, CardStrength> STRENGTHS_PART_2 = {
        {'A', 14},
        {'K', 13},
        {'Q', 12},
        {'T', 10},
        {'9', 9},
        {'8', 8},
        {'7', 7},
        {'6', 6},
        {'5', 5},
        {'4', 4},
        {'3', 3},
        {'2', 2},
        {'J', 1},
};

typedef enum HandType {
    HAND_TYPE_FIVE_OF_KIND = 5,
    HAND_TYPE_FOUR_OF_KIND = 4,
    HAND_TYPE_FULL_HOUSE = 3,
    HAND_TYPE_THREE_OF_KIND = 2,
    HAND_TYPE_TWO_PAIR = 1,
    HAND_TYPE_PAIR = 0,
    HAND_TYPE_HIGH_CARD = -1,
} HandType;

typedef HandType HandTypeDeterminant(const std::vector<CardStrength> &cards);

HandType determine_hand_type(const std::vector<CardStrength> &cards) {
    std::map<CardStrength, std::size_t> card_counter;
    for (auto& card: cards) {
        card_counter[card]++;
    }

    std::vector<ulong> counted_cards(card_counter.size());
    std::transform(card_counter.begin(), card_counter.end(), counted_cards.begin(), [](const auto &pair) {
        return pair.second;
    });

    std::sort(counted_cards.begin(), counted_cards.end(), [](auto &a, auto &b) {
        return a > b;
    });

    if (counted_cards[0] == 5) {
        return HAND_TYPE_FIVE_OF_KIND;
    } else if (counted_cards[0] == 4) {
        return HAND_TYPE_FOUR_OF_KIND;
    } else if (counted_cards[0] == 3 && counted_cards[1] == 2) {
        return HAND_TYPE_FULL_HOUSE;
    } else if (counted_cards[0] == 3) {
        return HAND_TYPE_THREE_OF_KIND;
    } else if (counted_cards[0] == 2 && counted_cards[1] == 2) {
        return HAND_TYPE_TWO_PAIR;
    } else if (counted_cards[0] == 2) {
        return HAND_TYPE_PAIR;
    } else {
        return HAND_TYPE_HIGH_CARD;
    }
}

HandType determine_hand_type_part_2(const std::vector<CardStrength> &cards) {
    std::map<CardStrength, std::size_t> card_counter;
    for (auto& card: cards) {
        card_counter[card]++;
    }

    if (card_counter.contains(STRENGTHS_PART_2.at('J'))) {
        // TODO Possible to do with std::permutation or Heap's algorithm
        // TODO This is painfully slow, but works

        // Find the joker
        auto joker_pos = std::distance(cards.begin(), std::find(cards.begin(), cards.end(), STRENGTHS_PART_2.at('J')));
        std::vector<CardStrength> joker_substitutions(STRENGTHS_PART_2.size());
        std::transform(STRENGTHS_PART_2.begin(), STRENGTHS_PART_2.end(), joker_substitutions.begin(), [](auto &pair) {
            return pair.second;
        });
        joker_substitutions.erase(std::remove_if(joker_substitutions.begin(), joker_substitutions.end(), [](auto &c) {
            return STRENGTHS_PART_2.at('J') == c;
        }), joker_substitutions.end());

        std::vector<HandType> possible_hand_types(joker_substitutions.size());
        std::transform(joker_substitutions.begin(), joker_substitutions.end(), possible_hand_types.begin(), [cards, &joker_pos](auto &sub) {
            std::vector<CardStrength> cards_copy(cards);
            cards_copy[joker_pos] = sub;
            return determine_hand_type_part_2(cards_copy);
        });

        return *std::max_element(possible_hand_types.begin(), possible_hand_types.end());
    } else {
        return determine_hand_type(cards);
    }
}

typedef struct Game {
    ulong bid = 0;
    HandType hand_type = HAND_TYPE_HIGH_CARD;
    std::vector<ulong> cards_by_strength = {};
} Game;

std::vector<Game> parse_games(const std::string &in, const std::map<char, CardStrength> &card_strengths, HandTypeDeterminant determinant) {
    auto games_str = string_split(trim(in), '\n');
    std::vector<Game> games(games_str.size());

    std::transform(games_str.begin(), games_str.end(), games.begin(), [&determinant, &card_strengths](auto &game_str) {
        auto hand_and_bid = string_split(trim(game_str), ' ');
        if (hand_and_bid.size() != 2) {
            throw std::logic_error("The vector must be of size 2");
        }

        std::vector<CardStrength> cards_by_strength(5);
        std::transform(hand_and_bid.at(0).begin(), hand_and_bid.at(0).end(), cards_by_strength.begin(), [&card_strengths](auto c) {
            return card_strengths.at(c);
        });

        ulong bid = std::stoul(trim(hand_and_bid.at(1)));

        return Game{bid, determinant(cards_by_strength), cards_by_strength};
    });

    return games;
}

bool compare_games(const Game &a, const Game &b) {
    if (a.hand_type == b.hand_type) {
        for (std::size_t i = 0; i < 5; i++) {
            if (a.cards_by_strength[i] != b.cards_by_strength[i]) {
                return a.cards_by_strength[i] < b.cards_by_strength[i];
            }
        }

        throw std::logic_error("The hands are identical and there is no instruction for handling");
    }

    return a.hand_type < b.hand_type;
}

std::vector<ulong> determine_game_bids(const std::vector<Game>& games) {
    std::vector<ulong> game_bids;
    game_bids.reserve(games.size());
    for (std::size_t idx = 0; idx < games.size(); idx++) {
        game_bids.push_back(games[idx].bid * (idx + 1));
    }
    return game_bids;
}

SOLVER(2023, 7, 1, true)
(const std::string &in) {
    // auto games = parse_games(std::string(EXAMPLE_1), STRENGTHS, determine_hand_type);
    auto games = parse_games(std::string(in), STRENGTHS, determine_hand_type);
    std::sort(games.begin(), games.end(), compare_games);

    auto game_bids = determine_game_bids(games);
    ulong sum = std::accumulate(game_bids.begin(), game_bids.end(), 0ul, [](auto acc, auto val) {
        return acc + val;
    });

    return fmt::format("{}", sum);
}

SOLVER(2023, 7, 2, true)
(const std::string &in) {
//     auto games = parse_games(std::string(EXAMPLE_1), STRENGTHS_PART_2, determine_hand_type_part_2);
    auto games = parse_games(std::string(in), STRENGTHS_PART_2, determine_hand_type_part_2);
    std::sort(games.begin(), games.end(), compare_games);

    auto game_bids = determine_game_bids(games);
    ulong sum = std::accumulate(game_bids.begin(), game_bids.end(), 0ul, [](auto acc, auto val) {
        return acc + val;
    });

    return fmt::format("{}", sum);
}
