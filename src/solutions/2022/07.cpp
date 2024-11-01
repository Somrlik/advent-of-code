#include <map>
#include "../one_solution.h"
#include "../../trim.h"
#include "../string_split.h"

#include <sstream>
#include <stack>

constexpr const std::string_view EXAMPLE_INPUT_1 = R"(
$ cd /
$ ls
dir a
14848514 b.txt
8504156 c.dat
dir d
$ cd a
$ ls
dir e
29116 f
2557 g
62596 h.lst
$ cd e
$ ls
584 i
$ cd ..
$ cd ..
$ cd d
$ ls
4060174 j
8033020 d.log
5626152 d.ext
7214296 k
)";

namespace {
    enum RecordType {
        TYPE_FILE,
        TYPE_DIRECTORY,
    };

    struct FilesystemRecord {
        RecordType type;
        std::string name;
        std::size_t size;
        std::vector<FilesystemRecord> records;
    };

    enum Command {
        START,
        LS,
        CD,
    };

    struct ShellCommand {
        Command command;
        std::string argument;
        std::vector<std::string> output_lines;
    };
}

static const std::map<std::string, Command> string_to_command {
    {"ls", LS},
    {"cd", CD},
};

static std::vector<ShellCommand> parse_shell_commands(const std::string &in) {
    std::string line;
    std::istringstream stream(trim(in));
    std::vector<ShellCommand> shell_commands;

    ShellCommand current_command{START};
    while (std::getline(stream, line)) {
        if (line.empty()) continue;

        if (line.at(0) == '$') {
            shell_commands.push_back(current_command);
            current_command.output_lines.clear();

            current_command.command = string_to_command.at(std::string(line.begin() + 2, line.begin() + 4));
            if (current_command.command == CD) {
                current_command.argument = std::string(line.begin() + 5, line.end());
            }
        } else {
            current_command.output_lines.push_back(line);
        }
    }
    shell_commands.push_back(current_command);

    return shell_commands;
}

static FilesystemRecord build_filesystem_from_shell_command(const std::vector<ShellCommand>& shell_commands) {
    FilesystemRecord root{TYPE_DIRECTORY, "/", 0, {}};
    std::stack<std::reference_wrapper<FilesystemRecord>> cwd;
    cwd.push(std::ref(root));

    for (const auto &shell_command: shell_commands) {
        if (shell_command.command == START) continue;

        if (shell_command.command == CD) {
            if (shell_command.argument == "/") {
                while (!cwd.empty()) cwd.pop();
                cwd.push(std::ref(root));
                continue;
            }

            if (shell_command.argument == "..") {
                cwd.pop();
                continue;
            }

            auto found = std::find_if(cwd.top().get().records.begin(), cwd.top().get().records.end(), [&shell_command](auto const &record) {
                return shell_command.argument == record.name;
            });

            if (found == cwd.top().get().records.end()) throw std::logic_error("Failed to find dir with name " + shell_command.argument);
            cwd.push(std::ref(*found));
        }

        if (shell_command.command == LS) {
            for (auto const& line: shell_command.output_lines) {
                auto size_name = string_split(line, ' ');
                FilesystemRecord record;
                if (size_name.at(0) == "dir") {
                    record.type = TYPE_DIRECTORY;
                    record.size = 0;
                    record.records = {};
                } else {
                    record.type = TYPE_FILE;
                    record.size = std::stoul(size_name.at(0));
                }

                record.name = size_name.at(1);
                cwd.top().get().records.push_back(record);
            }
        }
    }

    return root;
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"
static void fill_in_directory_sizes(FilesystemRecord &root) {
    for (auto &record: root.records) {
        if (record.type == TYPE_FILE) {
            root.size += record.size;
            continue;
        }

        if (record.type == TYPE_DIRECTORY) {
            if (record.size == 0) {
                fill_in_directory_sizes(record);
            }
            root.size += record.size;
        }
    }
}
#pragma clang diagnostic pop



SOLVER(2022, 7, 1, true)
(const std::string &in) {
    auto shell_commands = parse_shell_commands(in);
//    auto shell_commands = parse_shell_commands(std::string(EXAMPLE_INPUT_1));

    auto filesystem = build_filesystem_from_shell_command(shell_commands);
    fill_in_directory_sizes(filesystem);

    std::size_t sum = 0;

    std::stack<FilesystemRecord> directories;
    directories.push(filesystem);
    while (!directories.empty()) {
        auto record = directories.top();
        directories.pop();

        // Do something with value
        if (record.size <= 100'000) {
            sum += record.size;
        }

        for(const auto& sub_record: record.records) {
            if (sub_record.type == TYPE_DIRECTORY) {
                directories.push(sub_record);
            }
        }
    }

    return fmt::format("{}", sum);
}

SOLVER(2022, 7, 2, true)
(const std::string &in) {
    const long FS_TOTAL = 70'000'000;
    const long FS_NEEDED = 30'000'000; // 43'562'874

    auto shell_commands = parse_shell_commands(in);
//    auto shell_commands = parse_shell_commands(std::string(EXAMPLE_INPUT_1));

    auto filesystem = build_filesystem_from_shell_command(shell_commands);
    fill_in_directory_sizes(filesystem);

    const long currently_unused = FS_TOTAL - (long)filesystem.size;
    const long currently_needed = FS_NEEDED - currently_unused;
    long smallest_diff = std::numeric_limits<long>::max();

    std::stack<FilesystemRecord> directories;
    directories.push(filesystem);
    while (!directories.empty()) {
        auto record = directories.top();
        directories.pop();

        // Do something with value
        if (record.size > currently_needed) {
            smallest_diff = std::min(smallest_diff, (long)record.size);
        }

        for(const auto& sub_record: record.records) {
            if (sub_record.type == TYPE_DIRECTORY) {
                directories.push(sub_record);
            }
        }
    }

    // 43562874 too high
    return fmt::format("{}", smallest_diff);
}
