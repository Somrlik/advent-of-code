Advent of Code Solutions by somrlik
===================================

These are my solutions for https://adventofcode.com.

Also includes a small CLI for running and downloading of puzzle inputs.

Compiling
---------

Compiles on `GNU 11.4.0` for linux, might compile for other targets.

- Requires
  - `C++20` compiler
  - `cmake`
- Optionally requires
  - `curl` for downloading of puzzle inputs
  - `openssl` for some puzzles

1. `cmake -S . -B build`
2. `cmake --build build`
3. `./build/aoc-test`

Running
-------
```
./build/aoc --help
```

License
-------

MIT
