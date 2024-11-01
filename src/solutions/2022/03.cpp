#include <numeric>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
vJrwpWtwJgWrhcsFMMfFFhFp
jqHRNqRjqzjGDLGLrsFMfFZSrLrFZsSL
PmmdzqPrVvPwwTWBwg
wMqvLMZHhHMvwLHjbvcjnnSBnvTQFn
ttgJtRGJQctTZtZT
CrZsJsPPZsGzwwsLwLmpwMDw
)";

long get_priority_of_item(char c) {
    if (std::islower(c)) {
        return (int)c - 0x60;
    } else if (std::isupper(c)) {
        return (int)c - 0x40 + 26;
    }

    throw std::logic_error("The item must be lowercase or uppercase letter");
}

namespace {
    typedef struct Backpack {
        std::string left;
        std::string right;
    } Backpack;
}

static std::vector<Backpack> parse_backpacks(const std::string &in) {
    auto backpacks_str = string_split(trim(in), '\n');
    std::vector<Backpack> backpacks;
    for (const auto& backpack_str: backpacks_str) {
        if (backpack_str.size() % 2 == 1) throw std::logic_error("The backpack must be even sized");
        auto middle = backpack_str.size() / 2;
        backpacks.push_back(Backpack{backpack_str.substr(0, middle), backpack_str.substr(middle)});
    }
    return backpacks;
}

static char find_identical_item(const Backpack& backpack) {
    std::set<char> left_set(backpack.left.begin(), backpack.left.end());
    std::set<char> right_set(backpack.right.begin(), backpack.right.end());

    std::vector<char> intersection;
    std::set_intersection(left_set.begin(), left_set.end(), right_set.begin(), right_set.end(), std::back_inserter(intersection));

    if (intersection.size() != 1) {
        throw std::logic_error("The difference must be exactly one char");
    }

    return intersection.at(0);
}

SOLVER(2022, 3, 1, true)
(const std::string &in) {
    auto backpacks = parse_backpacks(in);
//    auto backpacks = parse_backpacks(std::string(EXAMPLE_INPUT_1));

    auto sum = std::accumulate(backpacks.begin(), backpacks.end(), 0ul, [](auto acc, auto const& backpack) {
        return acc + get_priority_of_item(find_identical_item(backpack));
    });

    return fmt::format("{}", sum);
}

namespace {
    typedef std::array<std::string, 3> BackpackGroup;
}

static std::vector<BackpackGroup> parse_backpack_groups(const std::string& in) {
    auto backpacks_str = string_split(trim(in), '\n');
    std::vector<BackpackGroup> groups(backpacks_str.size() / 3);

    for (int i = 0; i < backpacks_str.size(); ++i) {
        auto group_idx = i / 3;
        auto idx_in_group = i % 3;

        groups[group_idx][idx_in_group] = backpacks_str.at(i);
    }

    return groups;
}

static char find_identical_item(const BackpackGroup& group) {
    std::set<char> first_set(group.at(0).begin(), group.at(0).end());
    std::set<char> second_set(group.at(1).begin(), group.at(1).end());
    std::set<char> third_set(group.at(2).begin(), group.at(2).end());

    std::set<char> temp_set;
    std::set_intersection(first_set.begin(), first_set.end(), second_set.begin(), second_set.end(), std::inserter(temp_set, temp_set.begin()));
    second_set = std::move(temp_set);

    std::set_intersection(second_set.begin(), second_set.end(), third_set.begin(), third_set.end(), std::inserter(temp_set, temp_set.begin()));
    if (temp_set.size() != 1) {
        throw std::logic_error("The difference must be exactly one char");
    }

    return *temp_set.begin();
}

SOLVER(2022, 3, 2, true)
(const std::string &in) {
    auto backpack_groups = parse_backpack_groups(in);
//    auto backpack_groups = parse_backpack_groups(std::string(EXAMPLE_INPUT_1));

    auto sum = std::accumulate(backpack_groups.begin(), backpack_groups.end(), 0ul, [](auto acc, auto const& group) {
        return acc + get_priority_of_item(find_identical_item(group));
    });

    return fmt::format("{}", sum);
}
