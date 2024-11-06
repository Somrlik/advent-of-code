#include <variant>
#include <optional>
#include <stack>
#include "../one_solution.h"

#pragma region Example Inputs
constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
[1,1,3,1,1]
[1,1,5,1,1]

[[1],[2,3,4]]
[[1],4]

[9]
[[8,7,6]]

[[4,4],4,4]
[[4,4],4,4,4]

[7,7,7,7]
[7,7,7]

[]
[3]

[[[]]]
[[]]

[1,[2,[3,[4,[5,6,7]]]],8,9]
[1,[2,[3,[4,[5,6,0]]]],8,9]
)";
#pragma endregion

namespace {
    struct Tree;
}

static std::strong_ordering compare_trees(const Tree &a, const Tree &b);

namespace {
    enum NodeType {
        TREE,
        LEAF,
    };

    struct Tree {
        NodeType type;
        int value;
        std::vector<Tree> leaves;
        std::string in;

        auto operator <=>(const Tree &other) const {
            return compare_trees(*this, other);
        }
    };
}

static Tree build_tree(const auto &in) {
    Tree root = {TREE, -1, {}, in};
    std::stack<std::reference_wrapper<Tree>> stack;
    stack.push(std::ref(root));

    auto it = in.begin() + 1; // skip the first [ since that is the root
    std::string number_acc;
    while (it != in.end()) {
        if (*it == '[') {
            Tree new_leaf = {TREE, -1, {}};
            Tree *stack_top = &stack.top().get();
            stack_top->leaves.push_back(new_leaf);
            stack.push(std::ref(*(stack_top->leaves.end() - 1)));
            number_acc.erase();
        } else if (*it == ']') {
            if (!number_acc.empty()) {
                Tree new_leaf = Tree{LEAF, std::stoi(number_acc), {}};
                stack.top().get().leaves.push_back(new_leaf);
            }
            stack.pop();
            number_acc.erase();
        } else if (*it == ',') {
            if (!number_acc.empty()) {
                Tree new_leaf = Tree{LEAF, std::stoi(number_acc), {}};
                stack.top().get().leaves.push_back(new_leaf);
            }
            number_acc.erase();
        } else if (std::isdigit(*it)) {
            number_acc += *it;
        }

        ++it;
    }

    return root;
}

static std::vector<std::array<Tree, 2>> build_pairs(const std::string &in) {
    auto pairs_str = string_split(trim(in), "\n\n");
    std::vector<std::array<Tree, 2>> pairs;
    for (const auto& pair_str: pairs_str) {
        std::array<Tree, 2> trees;
        // Left, right
        auto two_trees_str = string_split(trim(pair_str), '\n');
        std::transform(two_trees_str.begin(), two_trees_str.end(), trees.begin(), [](const auto &tree_str) {
            return build_tree(trim(tree_str));
        });
        pairs.push_back(trees);
    }

    return pairs;
}

static std::strong_ordering compare_trees(const Tree &a, const Tree &b) {
    if (a.type == LEAF && b.type == LEAF) {
        return a.value <=> b.value;
    }

    if (a.type == TREE && b.type == TREE) {
        for (int i = 0; i < std::max(a.leaves.size(), b.leaves.size()); ++i) {
            if (i == a.leaves.size()) {
                return std::strong_ordering::less;
            } else if (i == b.leaves.size()) {
                return std::strong_ordering::greater;
            }

            auto result = compare_trees(a.leaves[i], b.leaves[i]);
            if (result == std::strong_ordering::greater) {
                return std::strong_ordering::greater;
            } else if (result == std::strong_ordering::less) {
                return std::strong_ordering::less;
            }
        }
    }

    if (a.type == LEAF && b.type == TREE) {
        return compare_trees(Tree{TREE, -1, {Tree{LEAF, a.value}}}, b);
    } else if (a.type == TREE && b.type == LEAF) {
        return compare_trees(a, Tree{TREE, -1, {Tree{LEAF, b.value}}});
    }

    return std::strong_ordering::equal;
}

SOLVER(2022, 13, 1, true)
(const std::string &in) {
    auto trees = build_pairs(in);
//    auto trees = build_pairs(std::string(EXAMPLE_INPUT_1));

    long sum = 0;
    for (int idx = 0; idx < trees.size(); ++idx) {
        auto left = trees.at(idx).at(0);
        auto right = trees.at(idx).at(1);
        auto result = compare_trees(left, right);
        if (result == std::strong_ordering::less) {
            fmt::println("result: less GOOD");
            sum += idx + 1;
        } else if (result == std::strong_ordering::greater) {
            fmt::println("result: greater BAD");
        } else if (result == std::strong_ordering::equal || result == std::strong_ordering::equivalent) {
            throw std::logic_error("They are not supposed to be equal");
        }
    }

    return fmt::format("{}", sum);
}

SOLVER(2022, 13, 2, true)
(const std::string &in) {
//    auto all_packets = string_split(trim(std::string(EXAMPLE_INPUT_1)), '\n');
     auto all_packets = string_split(trim(in), '\n');

    all_packets.erase(std::remove_if(all_packets.begin(), all_packets.end(), [](const auto& str) {
        return str.empty() || str == "\n";
    }), all_packets.end());

    // Add the additional packets
    const std::array<std::string, 2> additional_packets = {
            "[[2]]",
            "[[6]]",
    };
    all_packets.insert(all_packets.end(), additional_packets.begin(), additional_packets.end());

    std::vector<Tree> all_trees(all_packets.size());
    std::transform(all_packets.begin(), all_packets.end(), all_trees.begin(), [](auto const &packet) {
        return build_tree(packet);
    });

    std::sort(all_trees.begin(), all_trees.end());

    std::array<long, 2> positions = {};
    std::transform(additional_packets.begin(), additional_packets.end(), positions.begin(), [&all_trees](const auto &to_find) {
        auto found = std::find_if(all_trees.begin(), all_trees.end(), [&to_find](auto const &item) {
            return item.in == to_find;
        });
        if (found == all_trees.end()) throw std::logic_error("the packet was not found");
        return found - all_trees.begin() + 1;
    });

    // 90902 is too high, I removed the call to sort by accident lol
    return fmt::format("{}", positions.at(0) * positions.at(1));
}
