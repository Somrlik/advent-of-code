#ifndef AOC_ONE_SOLUTION_H
#define AOC_ONE_SOLUTION_H

#include "../solutions.h"

#include <string>
#include <fmtlog.h>

// This indirection is required
#define CONCATENATE(x, y) x##y
#define MAKE_UNIQUE_NAME(x, y) CONCATENATE(x, y)
#define ADD_SOLUTION_MAKE_NAME MAKE_UNIQUE_NAME(s_, __LINE__)
#define ADD_SOLUTION(year, day, level, ptr, solved) static inline add_solution ADD_SOLUTION_MAKE_NAME ( { year, day, level, ptr, solved } )

#define CREATE_SOLVER_NAME MAKE_UNIQUE_NAME(solver_, __LINE__)

#define SOLVER(year, day, level, solved) \
    static std::string CREATE_SOLVER_NAME (const std::string &in); \
    ADD_SOLUTION(year, day, level, &CREATE_SOLVER_NAME , solved);    \
    static std::string CREATE_SOLVER_NAME

#endif
