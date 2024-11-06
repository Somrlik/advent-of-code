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
    - Can be specified with `-DUSE_CURL`
  - `openssl` for some puzzles
    - Can be specified with `-DUSE_OPENSSL`
- Optional dependencies 


1. `cmake -S . -B build`
2. `cmake --build build`
3. `./build/aoc-test`

Running
-------

Upon first run without supplying the input file, you will be asked to 
provide your aoc session cookie. It is then saved in `${XDG_DATA_HOME}/.aoc` or in `~/.aoc`.

No clue where it would be saved on windows.

```
./build/aoc --help
```

License
-------

MIT
