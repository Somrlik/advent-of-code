#ifndef SOLUTIONS_H
#define SOLUTIONS_H

#include <string>
#include <set>
#include <memory>
#include <vector>

using SolutionFunc = std::string (*)(const std::string&);

struct Solution {
    int year;
    int day;
    int level;
    SolutionFunc solution_func;
    bool solved;
};

struct add_solution {
    explicit add_solution(const Solution&);
    ~add_solution();
private:
    // Do not copy the struct
    add_solution(const add_solution &other);
};

bool is_solved(uint year, uint day, uint level);
bool has_solver(uint year, uint day, uint level);
std::string run_solver(uint year, uint day, uint level, const std::string &input);

#endif
