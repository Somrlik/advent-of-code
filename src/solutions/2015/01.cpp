#include "../one_solution.h"

SOLVER(2015, 1, 1, true)
(const std::string &in) {
    int floor = 0;
    for (auto c : in) {
        if (c == '(') {
            floor++;
        } else {
            floor--;
        }
    }

    return fmt::format("{}", floor);
}

SOLVER(2015, 1, 2, true)
(const std::string &in) {
    int floor = 0;
    int i = 1;

    for (auto c : in) {
        if (c == '(') {
            floor++;
        } else {
            floor--;
        }

        if (floor == -1) {
            return fmt::format("{}", i);
        }
        i++;
    }

    return "n/a";
}
