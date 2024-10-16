#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

#include <algorithm>
#include <numeric>

constexpr const std::string_view EXAMPLE_1 = R"(
Card 1: 41 48 83 86 17 | 83 86  6 31 17  9 48 53
Card 2: 13 32 20 16 61 | 61 30 68 82 17 32 24 19
Card 3:  1 21 53 59 44 | 69 82 63 72 16 21 14  1
Card 4: 41 92 73 84 69 | 59 84 76 51 58  5 54 83
Card 5: 87 83 26 28 32 | 88 30 70 12 93 22 82 36
Card 6: 31 18 13 56 72 | 74 77 10 23 35 67 36 11
)";

typedef struct Card {
    uint card_id;
    uint copies;
    std::vector<uint> winning_numbers;
    std::vector<uint> my_numbers;
} Card;

std::vector<Card> parse_cards(const std::string& in) {
    const auto lines = string_split(trim(in), '\n');
    std::vector<Card> cards;
    for (const auto &line: lines) {
        const auto card_id_and_numbers = string_split(line, ':');
        if (card_id_and_numbers.size() != 2) {
            throw std::logic_error("The size of the vector must be 2");
        }
        auto card_id_str = string_split(card_id_and_numbers.at(0), ' ');
        card_id_str.erase(std::remove_if(
                card_id_str.begin(),
                card_id_str.end(),
                [](const auto &item) {
                    return item.empty();
                }
        ), card_id_str.end());
        uint card_id = std::stoul(card_id_str.at(1));

        const auto winning_and_my_numbers = string_split(card_id_and_numbers.at(1), '|');
        if (winning_and_my_numbers.size() != 2) {
            throw std::logic_error("The size of the vector must be 2");
        }

        auto winning_numbers_str = string_split(winning_and_my_numbers.at(0), ' ');

        // Remove empty strings
        winning_numbers_str.erase(std::remove_if(
                winning_numbers_str.begin(),
                winning_numbers_str.end(),
                [](const auto &item) {
                    return item.empty();
                }
        ), winning_numbers_str.end());

        // Make uint from string
        std::vector<uint> winning_numbers(winning_numbers_str.size());
        std::transform(
                winning_numbers_str.begin(),
                winning_numbers_str.end(),
                winning_numbers.begin(),
                [](const auto &item) {
                    return std::stoul(item);
                }
        );

        auto my_numbers_str = string_split(winning_and_my_numbers.at(1), ' ');

        // Remove empty strings
        my_numbers_str.erase(std::remove_if(
                my_numbers_str.begin(),
                my_numbers_str.end(),
                [](const auto &item) {
                    return item.empty();
                }
        ), my_numbers_str.end());

        // Make uint from string
        std::vector<uint> my_numbers(my_numbers_str.size());
        std::transform(
                my_numbers_str.begin(),
                my_numbers_str.end(),
                my_numbers.begin(),
                [](const auto& item) {
                    return std::stoul(item);
                }
        );

        std::sort(winning_numbers.begin(), winning_numbers.end());
        std::sort(my_numbers.begin(), my_numbers.end());

        cards.push_back({
            card_id,
            1,
            winning_numbers,
            my_numbers
        });
    }

    return cards;
}

SOLVER(2023, 4, 1, true)
(const std::string &in) {
    auto cards = parse_cards(in);

    std::vector<ulong> points_per_card(cards.size());
    std::transform(cards.begin(), cards.end(), points_per_card.begin(), [](const auto& card) {
        // Find intersection, the numbers are already sorted
        std::vector<uint> intersection;
        std::set_intersection(
                card.winning_numbers.begin(),
                card.winning_numbers.end(),
                card.my_numbers.begin(),
                card.my_numbers.end(),
                std::back_inserter(intersection)
        );

        if (intersection.empty()) return 0ul;
        return (ulong)std::pow(2, intersection.size() - 1);
    });

    ulong sum = std::accumulate(points_per_card.begin(), points_per_card.end(), 0ul, [](ulong acc, const auto& number) {
        return acc + number;
    });

    return fmt::format("{}", sum);
}

SOLVER(2023, 4, 2, true)
(const std::string &in) {
//    auto cards = parse_cards(std::string(EXAMPLE_1));
    auto cards = parse_cards(in);

    const std::size_t max_card_idx = cards.size();
    for (std::size_t card_idx = 0; card_idx < max_card_idx; card_idx++) {
        const auto card = cards.at(card_idx);
        // Find intersection, the numbers are already sorted
        std::vector<uint> intersection;
        std::set_intersection(
                card.winning_numbers.begin(),
                card.winning_numbers.end(),
                card.my_numbers.begin(),
                card.my_numbers.end(),
                std::back_inserter(intersection)
        );

        for (std::size_t card_count = 0; card_count < intersection.size(); card_count++) {
             const std::size_t idx = card_idx + card_count + 1;
             if (idx >= max_card_idx) {
                 // Check that we are not out of range in the cards vector
                 break;
             }

             auto card_to_add_copy_of = &cards.at(idx);
             card_to_add_copy_of->copies += card.copies;
        }
    }

    ulong copies_count = std::accumulate(cards.begin(), cards.end(), 0ul, [](auto acc, const auto& card) {
        return acc + card.copies;
    });

    return fmt::format("{}", copies_count);
}
