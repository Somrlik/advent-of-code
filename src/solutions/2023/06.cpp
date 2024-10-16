#include <numeric>
#include "../one_solution.h"
#include "../string_split.h"
#include "../../trim.h"

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
Time:      7  15   30
Distance:  9  40  200
)";

typedef struct Race {
    ulong time;
    ulong distance;
} Race;

std::vector<Race> parse_races(const std::string &in) {
    const auto times_and_distances = string_split(trim(in), '\n');
    auto times_str = string_split(trim(times_and_distances.at(0)), ':').at(1);
    std::vector<uint> times;
    for (auto& time_str: string_split(times_str, ' ')) {
        if (time_str.empty()) continue;
        times.push_back(std::stoul(time_str));
    }
    
    auto distances_str = string_split(trim(times_and_distances.at(1)), ':').at(1);
    std::vector<uint> distances;
    for (auto& distance_str: string_split(distances_str, ' ')) {
        if (distance_str.empty()) continue;
        distances.push_back(std::stoul(distance_str));
    }

    if (times.size() != distances.size()) {
        throw std::logic_error("The vectors must be of the same length");
    }

    std::vector<Race> races;
    for (std::size_t i = 0; i < times.size(); i++) {
        races.push_back({times.at(i), distances.at(i)});
    }

    return races;
}

std::vector<ulong> precompute_distances(ulong time) {
    // total time will be x
    // held down time will be y
    // speed is determined by how long the button is held down -> y
    // distance traveled is speed times remaining time -> y * (x - y)

    std::vector<ulong> out;
    for (std::size_t i = 0; i < time; i++) {
        auto ans = i * (time - i);
        out.push_back(ans);
    }
    return out;
}

ulong get_solution_product(const std::vector<Race> &races) {
    std::vector<ulong> solutions(races.size());
    std::transform(races.begin(), races.end(), solutions.begin(), [](auto &race) {
        auto distances = precompute_distances(race.time);
        return std::count_if(distances.begin(), distances.end(), [&race](auto distance) {
            return distance > race.distance;
        });
    });

    return std::accumulate(solutions.begin(), solutions.end(), 1ul, [](auto acc, auto &solution) {
        return acc * solution;
    });
}

SOLVER(2023, 6, 1, true)
(const std::string &in) {
//    auto races = parse_races(std::string(EXAMPLE_INPUT_1));
    auto races = parse_races(in);
    return fmt::format("{}", get_solution_product(races));
}

SOLVER(2023, 6, 2, true)
(const std::string &in) {
//    auto races = parse_races(std::string(EXAMPLE_INPUT_1));
    auto races = parse_races(in);

    std::string time_buf;
    std::string distance_buf;

    for (auto& race: races) {
        time_buf.append(std::to_string(race.time));
        distance_buf.append(std::to_string(race.distance));
    }

    const std::vector<Race> combined_race = {
        {std::stoul(time_buf), std::stoul(distance_buf)}
    };

    return fmt::format("{}", get_solution_product(combined_race));
}
