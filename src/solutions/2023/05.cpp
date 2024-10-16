#include "../one_solution.h"

#include <map>
#include <ranges>
#include <numeric>
#include <limits>
#include <future>
#include "../../trim.h"
#include "../string_split.h"

constexpr const std::string_view EXAMPLE_1 = R"(
seeds: 79 14 55 13

seed-to-soil map:
50 98 2
52 50 48

soil-to-fertilizer map:
0 15 37
37 52 2
39 0 15

fertilizer-to-water map:
49 53 8
0 11 42
42 0 7
57 7 4

water-to-light map:
88 18 7
18 25 70

light-to-temperature map:
45 77 23
81 45 19
68 64 13

temperature-to-humidity map:
0 69 1
1 0 69

humidity-to-location map:
60 56 37
56 93 4
)";

typedef struct MapItem {
    ulong source_start;
    ulong destination_start;
    ulong length;
} MapItem;

typedef struct Map {
    std::string name;
    const std::vector<MapItem> items;
} Map;

struct parse_maps_return {
    std::vector<uint> seeds;
    std::map<std::string, Map> maps;
};

struct parse_maps_return parse_maps(const std::string& in) {
    auto splits = string_split(trim(in), "\n\n");

    auto seeds_str = string_split(string_split(splits.at(0), ':').at(1), ' ');
    seeds_str.erase(std::remove_if(seeds_str.begin(), seeds_str.end(), [](const auto& item) {
        return item.empty();
    }), seeds_str.end());
    std::vector<uint> seeds(seeds_str.size());
    std::transform(
            seeds_str.begin(),
            seeds_str.end(),
            seeds.begin(),
            [](const auto& str) {
                return std::stoul(str);
            }
    );

    // Erase seeds
    splits.erase(splits.begin());

    // Make maps
    std::map<std::string, Map> maps;
    std::for_each(splits.begin(), splits.end(), [&maps](const auto &split) {
        const auto name_and_values = string_split(trim(split), ':');
        if (name_and_values.size() != 2) {
            throw std::logic_error("The vector must be of size 2");
        }

        auto name = string_split(name_and_values.at(0), ' ').at(0);
        auto values = string_split(trim(name_and_values.at(1)), '\n');
        std::vector<MapItem> map_items;
        std::for_each(values.begin(), values.end(), [&map_items](auto const &value) {
            auto values_splits = string_split(value, ' ');
            if (values_splits.size() != 3) {
                throw std::logic_error("The vector must be of size 3");
            }

            map_items.push_back({
                std::stoul(values_splits.at(1)),
                std::stoul(values_splits.at(0)),
                std::stoul(values_splits.at(2)),
            });
        });

        std::sort(map_items.begin(), map_items.end(), [](const auto &a, const auto &b) {
            return a.source_start < b.source_start;
        });

        maps.emplace(name, Map{name, map_items});
    });

    return {seeds, maps};
}

ulong determine_seed_location(ulong seed, const std::vector<Map> &map_stack) {
    ulong current_source = seed;
    for (auto const& stack_item: map_stack) {
        // Find which MapItem corresponds to next value
        std::size_t map_item_idx = 0;
        for (; map_item_idx < stack_item.items.size(); map_item_idx++) {
            const auto map_item = stack_item.items.at(map_item_idx);
            // TODO This might be >= and also check for 0
            if (map_item.source_start > current_source) {
                if (map_item_idx != 0) map_item_idx--;
                break;
            }
        }

        if (map_item_idx == stack_item.items.size()) {
            // It still might be the last one
            map_item_idx--;
        }

        const auto map_item = stack_item.items.at(map_item_idx);
        if (current_source > (map_item.source_start + map_item.length)) {
            // Do not change the number
            current_source = current_source;
        } else if (current_source < map_item.source_start) {
            // Do not change the number
            current_source = current_source;
        } else {
            // Move to destination
            current_source = map_item.destination_start + (current_source - map_item.source_start);
        }
    }
    return current_source;
}

SOLVER(2023, 5, 1, true)
(const std::string &in) {
    //const auto [seeds, maps] = parse_maps(std::string(EXAMPLE_1));
    const auto [seeds, maps] = parse_maps(in);

    std::vector<Map> map_stack = {
            maps.at("seed-to-soil"),
            maps.at("soil-to-fertilizer"),
            maps.at("fertilizer-to-water"),
            maps.at("water-to-light"),
            maps.at("light-to-temperature"),
            maps.at("temperature-to-humidity"),
            maps.at("humidity-to-location"),
    };

    std::vector<ulong> seeds_locations;

    std::for_each(seeds.begin(), seeds.end(), [&map_stack, &seeds_locations](const auto& seed) {
        seeds_locations.emplace_back(determine_seed_location(seed, map_stack));
    });

    ulong lowest = *std::min_element(seeds_locations.begin(), seeds_locations.end());
    return fmt::format("{}", lowest);
}

ulong brute_force(const std::vector<std::pair<ulong, ulong>> &seeds_pairs, const std::vector<Map> &map_stack) {
    // Generate all seeds using ranges
    auto all_seeds = seeds_pairs | std::ranges::views::transform([](auto const& pair) {
        return std::ranges::views::iota(pair.first, pair.first + pair.second);
    }) | std::ranges::views::join;

    // Get total seed count for fun
    ulong total_seeds = std::accumulate(seeds_pairs.begin(), seeds_pairs.end(), 0ul, [](auto acc, auto const& pair) {
        return acc + pair.second;
    });

    // Determine seed locations
    ulong count = 0;

    // TODO: count is not thread-safe
    std::thread printing_thread = std::thread([&total_seeds, &count]() {
        ulong previous_count = count;

        while (count < total_seeds) {
            auto const seeds_per_second = (count - previous_count + 1) / 1;
            auto const time_remaining = (total_seeds - count) / seeds_per_second;
            fmt::println("{}/{} or {:.2f}% done {}s remain {} seeds per second", count, total_seeds, ((double)count/(double)total_seeds) * 100, time_remaining, seeds_per_second);

            previous_count = count;
            std::this_thread::sleep_for(std::chrono::seconds (1));
        }
    });

    ulong lowest = std::numeric_limits<ulong>::max();
    for (auto const& seed: all_seeds) {
        auto location = determine_seed_location(seed, map_stack);
        if (location < lowest) lowest = location;
        count++;
    }

    printing_thread.join();

    return lowest;
}

ulong brute_force_worker(ulong start, ulong length, const std::vector<Map> &map_stack) {
    ulong lowest = std::numeric_limits<ulong>::max();
    for (ulong i = 0; i < length; i++) {
        auto current = determine_seed_location(start + i, map_stack);
        if (current < lowest) lowest = current;
    }
    return lowest;
}

ulong brute_force_threaded(const std::vector<std::pair<ulong, ulong>> &seeds_pairs, const std::vector<Map> &map_stack) {
    std::vector<std::future<ulong>> seed_locations_futures;

    for (auto const &pair: seeds_pairs) {
        fmt::println("Starting thread with start {} length {}", pair.first, pair.second);
        seed_locations_futures.push_back(std::async(std::launch::async, brute_force_worker, pair.first, pair.second, map_stack));
    }

    std::vector<ulong> seed_locations(seed_locations_futures.size());
    std::transform(seed_locations_futures.begin(), seed_locations_futures.end(), seed_locations.begin(), [](auto& future) {
        auto value = future.get();
        return value;
    });

    return *std::min_element(seed_locations.begin(), seed_locations.end());
}

SOLVER(2023, 5, 2, true)
(const std::string &in) {
     const auto [seeds, maps] = parse_maps(std::string(EXAMPLE_1));
//    const auto [seeds, maps] = parse_maps(in);

    std::vector<Map> map_stack = {
            maps.at("seed-to-soil"),
            maps.at("soil-to-fertilizer"),
            maps.at("fertilizer-to-water"),
            maps.at("water-to-light"),
            maps.at("light-to-temperature"),
            maps.at("temperature-to-humidity"),
            maps.at("humidity-to-location"),
    };

    std::vector<std::pair<ulong, ulong>> seeds_pairs;
    for (std::size_t seeds_idx = 0; seeds_idx < seeds.size(); seeds_idx += 2) {
        seeds_pairs.emplace_back(seeds[seeds_idx], seeds[seeds_idx + 1]);
    }

    std::sort(seeds_pairs.begin(), seeds_pairs.end(), [](const auto& a, const auto& b) {
        return a.first < b.first;
    });

    // If any of them had an overlap, it would be possible to optimize for it, oh well

    // TODO The brute force solution is dumb and probably more difficult than actually doing it the proper way
    ulong lowest = brute_force(seeds_pairs, map_stack);
//    ulong lowest = brute_force_threaded(seeds_pairs, map_stack);
    return fmt::format("{}", lowest);
}
