#include "solutions.h"
#include <fmt/format.h>
#include <map>

typedef struct MapItem {
    SolutionFunc solver;
    bool solved;
} MapItem;

static auto solutions_map = std::make_unique<std::map<std::string, MapItem>>();

std::string identifiers_to_key(const uint year, const uint day, const uint level) {
    return fmt::format("{}-{}-{}", year, day, level);
}

static void add_solution_to_map(int year, int day, int level, SolutionFunc solution_func, bool solved) {
    auto key = fmt::format("{}-{}-{}", year, day, level);
    if (solutions_map->find(key) != solutions_map->end()) {
        fmt::println("Tried to specify two solutions for {}", key);
        throw std::exception();
    }
    solutions_map->insert({key, {solution_func, solved}});
}

add_solution::add_solution(const Solution &solution) {
    add_solution_to_map(solution.year, solution.day, solution.level, solution.solution_func, solution.solved);
}

add_solution::~add_solution() {
    solutions_map->clear();
}

bool has_solver(uint year, uint day, uint level) {
    auto key = identifiers_to_key(year, day, level);
    auto solution = solutions_map->find(key);

    return solution != solutions_map->end();
}

bool is_solved(uint year, uint day, uint level) {
    auto key = identifiers_to_key(year, day, level);
    auto solution = solutions_map->find(key);

    if (solution == solutions_map->end()) {
        return false;
    }

    return solution->second.solved;
}

std::string run_solver(uint year, uint day, uint level, const std::string &input) {
    auto key = identifiers_to_key(year, day, level);
    auto solution = solutions_map->find(key);

    if (solution == solutions_map->end()) {
        return fmt::format("Solution for {}/{}/{} not implemented.", year, day, level);
    }

    return solution->second.solver(input);
}
