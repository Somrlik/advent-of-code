#include <map>
#include <ranges>
#include <numeric>
#include "../one_solution.h"
#include "../string_split.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
rn=1,cm-,qp=3,cm=2,qp-,pc=4,ot=9,ab=5,pc-,pc=6,ot=7
)";

static std::uint8_t hash(const std::string &string) {
    long current = 0;
    for (const char &c: string) {
        current += (std::uint8_t) c;
        current *= 17;
        current %= 256;
    }
    return current;
}

SOLVER(2023, 15, 1, true)
(const std::string &in) {
    fmt::println("HASH hashes to {}", hash("HASH"));
    // auto input = std::string(EXAMPLE_INPUT_1) + "\n";
    auto input = in + "\n";
    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());

    auto splits = string_split(input, ',');
    ulong acc = 0;
    for (const auto& split: splits) {
        acc += hash(split);
    }

    return fmt::format("{}", acc);
}

void print_boxes(const std::map<std::uint8_t, std::vector<std::pair<std::string, long>>> &boxes) {
    for (auto const &[number, box_contents]: boxes) {
        if (box_contents.empty()) continue;

        fmt::print("Box {}: ", number);
        for (auto const &[label, focal_length]: box_contents) {
            fmt::print("[{} {}] ", label, focal_length);
        }
        fmt::print("\n");
    }
}

SOLVER(2023, 15, 2, true)
(const std::string &in) {
     auto input = in;
//    auto input = std::string(EXAMPLE_INPUT_1);

    input.erase(std::remove(input.begin(), input.end(), '\n'), input.end());
    auto instructions = string_split(input, ',');

    std::map<std::uint8_t, std::vector<std::pair<std::string, long>>> boxes{};
    for (std::weakly_incrementable auto n: std::views::iota(0, 256)) {
        boxes.insert({n, {}});
    }

    for (const auto& instruction: instructions) {
        std::string label;
        char operation;

        auto instruction_it = instruction.begin();
        for (;; ++instruction_it) {
            const auto c = *instruction_it;
            if (c == '-' || c == '=') {
                operation = c;
                break;
            }
            label += c;
        }

        std::uint8_t box_number = hash(label);
        auto& box = boxes.at(box_number);

        if (operation == '-') {
            // remove the lens
            box.erase(std::remove_if(box.begin(), box.end(),[&label](const auto &box_contents) {
                return box_contents.first == label;
            }), box.end());
        } else {
            long focal_length = std::stol(std::string(++instruction_it, instruction.end()));

            bool should_add = true;
            for (auto& box_contents: boxes.at(box_number)) {
                if (box_contents.first == label) {
                    box_contents.second = focal_length;
                    should_add = false;
                    break;
                }
            }

            if (should_add) {
                box.emplace_back(label, focal_length);
            }
        }
        fmt::println("After \"{}\":", instruction);
        print_boxes(boxes);
        fmt::print("\n");
    }

    long focusing_power = 0;
    for (auto const &[box_number, box_contents]: boxes) {
        for (long lens_idx = 0; lens_idx < box_contents.size(); ++lens_idx) {
            focusing_power += (box_number + 1) * (lens_idx + 1) * box_contents[lens_idx].second;
        }
    }

    return fmt::format("{}", focusing_power);
}
