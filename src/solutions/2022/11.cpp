#include "../one_solution.h"
#include "../BigInt.hpp"
#include <map>
#include <queue>
#include <numeric>

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
Monkey 0:
  Starting items: 79, 98
  Operation: new = old * 19
  Test: divisible by 23
    If true: throw to monkey 2
    If false: throw to monkey 3

Monkey 1:
  Starting items: 54, 65, 75, 74
  Operation: new = old + 6
  Test: divisible by 19
    If true: throw to monkey 2
    If false: throw to monkey 0

Monkey 2:
  Starting items: 79, 60, 97
  Operation: new = old * old
  Test: divisible by 13
    If true: throw to monkey 1
    If false: throw to monkey 3

Monkey 3:
  Starting items: 74
  Operation: new = old + 3
  Test: divisible by 17
    If true: throw to monkey 0
    If false: throw to monkey 1
)";
#pragma endregion

constexpr int OPERAND_OLD = std::numeric_limits<int>::min();

namespace {
    enum Operation {
        ADDITION,
        SUBTRACTION,
        MULTIPLICATION,
    };

    struct Monkey {
        uint id;
        std::deque<BigInt> items;
        Operation operation;
        /**
         * Can be a special value OPERAND_OLD in cases like "new = old * old"
         * @see OPERAND_OLD
         */
        int operand;
        uint divisibility_test;
        uint if_true;
        uint if_false;
        uint inspections = 0;
    };
}

static const std::map<char, Operation> char_to_operation {
    {'+', ADDITION},
    {'-', SUBTRACTION},
    {'*', MULTIPLICATION},
};

static std::map<uint, Monkey> parse_monkeys(const std::string &in) {
    auto monkeys_str = string_split(trim(in), "\n\n");
    std::map<uint, Monkey> monkeys;

    for (auto const &monkey_str: monkeys_str) {
        // Warning: hard-coded string idxes
        auto monkey_lines = string_split(trim(monkey_str), '\n');
        uint id = std::stoi(monkey_lines.at(0).substr(7, 8));

        auto items_str = string_split(std::string(monkey_lines.at(1).begin() + 18, monkey_lines.at(1).end()), ", ");
        std::deque<BigInt> items(items_str.size());
        std::transform(items_str.cbegin(), items_str.cend(), items.begin(), [](auto &item_str) {
            return BigInt(std::stol(item_str));
        });

        Operation operation = char_to_operation.at(std::string(monkey_lines.at(2).begin() + 23, monkey_lines.at(2).begin() + 24).at(0));
        auto operand_str = std::string(monkey_lines.at(2).begin() + 25, monkey_lines.at(2).end());
        int operand = OPERAND_OLD;
        if (operand_str != "old") {
            operand = std::stoi(operand_str);
        }


        uint test = std::stoi(std::string(monkey_lines.at(3).begin() + 21, monkey_lines.at(3).end()));
        uint if_true = std::stoi(std::string(monkey_lines.at(4).begin() + 29, monkey_lines.at(4).end()));
        uint if_false = std::stoi(std::string(monkey_lines.at(5).begin() + 30, monkey_lines.at(5).end()));

        monkeys.insert({id, Monkey{id, items, operation, operand, test, if_true, if_false}});
    }

    return monkeys;
}

void print_monkeys(const std::map<uint, Monkey>& monkeys) {
    for (const auto& [_, monkey]: monkeys) {
        std::string items;
        for (auto const &item: monkey.items) {
            items += fmt::format("{}, ", item);
        }

        fmt::println("Monkey {}: {}", monkey.id, items);
    }
}

static void do_monkey_business(std::map<uint, Monkey>& monkeys, uint rounds, bool divide_by_3) {
    long divisor_product = std::accumulate(monkeys.begin(), monkeys.end(), 1, [](auto acc, auto const &id_and_monkey) {
        return acc * id_and_monkey.second.divisibility_test;
    });

    for (int round = 0; round < rounds; ++round) {
        for (auto &[_, monkey]: monkeys) {

            while (!monkey.items.empty()) {
                auto item = monkey.items.front();
                monkey.items.pop_front();

                BigInt worry_level = item;
                auto operand = (monkey.operand == OPERAND_OLD ? item : BigInt{std::to_string(monkey.operand)});
                switch (monkey.operation) {
                    case ADDITION:
                        worry_level += operand;
                        break;
                    case SUBTRACTION:
                        worry_level -= operand;
                        break;
                    case MULTIPLICATION:
                        worry_level *= operand;
                        break;
                }
                if (divide_by_3) worry_level /= 3;
                else worry_level %= divisor_product;

                if (worry_level % monkey.divisibility_test == 0) {
                    monkeys.at(monkey.if_true).items.push_back(worry_level);
                } else {
                    monkeys.at(monkey.if_false).items.push_back(worry_level);
                }
                monkey.inspections++;
            }
        }
        if (round % 1000 == 0) {
            fmt::println("After round {}, the monkeys are holding items with these worry levels:", round);
            print_monkeys(monkeys);
        }
    }
}

SOLVER(2022, 11, 1, true)
(const std::string &in) {
//    auto monkeys = parse_monkeys(std::string(EXAMPLE_INPUT_1));
    auto monkeys = parse_monkeys(in);

    do_monkey_business(monkeys, 20, true);

    std::vector<uint> inspections(monkeys.size());
    std::transform(monkeys.begin(), monkeys.end(), inspections.begin(), [](auto const &id_and_monkey) {
        return id_and_monkey.second.inspections;
    });
    std::sort(inspections.begin(), inspections.end(), std::greater());
    auto product = inspections.at(0) * inspections.at(1);

    return fmt::format("{}", product);
}

SOLVER(2022, 11, 2, true)
(const std::string &in) {
//        auto monkeys = parse_monkeys(std::string(EXAMPLE_INPUT_1));
    auto monkeys = parse_monkeys(in);

    do_monkey_business(monkeys, 10'000, false);

    std::vector<long> inspections(monkeys.size());
    std::transform(monkeys.begin(), monkeys.end(), inspections.begin(), [](auto const &id_and_monkey) {
        return id_and_monkey.second.inspections;
    });
    std::sort(inspections.begin(), inspections.end(), std::greater());
    long product = inspections.at(0) * inspections.at(1);

    // 2232367972 too low, needs to be long
    return fmt::format("{}", product);
}
