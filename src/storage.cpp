#include "storage.h"
#include "fmtlog.h"
#include "trim.h"

#include <fstream>
#include <iostream>

constexpr const std::string_view STORAGE_DIR_NAME = ".aoc";
constexpr const std::string_view INPUTS_DIR_NAME = "inputs";
constexpr const std::string_view PROFILE_FILE_NAME = "profile";

using path = std::filesystem::path;

constexpr const std::filesystem::perms STORAGE_PERMISSIONS =
        std::filesystem::perms::all |
        std::filesystem::perms::group_read |
        std::filesystem::perms::others_read;

constexpr const std::filesystem::perms KEY_PERMISSIONS =
        std::filesystem::perms::owner_read | std::filesystem::perms::owner_write;

path determine_storage_dir_cache;

path determine_storage_dir() {
    if (!determine_storage_dir_cache.empty()) {
        return determine_storage_dir_cache;
    }

    path storage_dir;
    std::string xdg_home = std::getenv("XDG_DATA_HOME") ?: "";

    if (xdg_home.empty()) {
        logd("$XDG_DATA_HOME was empty, trying $HOME");

        std::string home = std::getenv("HOME") ?: "";
        if (home.empty()) {
            logd("$HOME was empty, using CWD");
            auto cwd = std::filesystem::current_path();
            // TODO Add handling for non-linux OSes
            logw("failed to determine persistent storage folder, most likely not linux, using $CWD ({})", cwd.string());
            storage_dir = cwd;
        } else {
            logd("setting storage root to $HOME({})", home);
            storage_dir = home;
        }
    } else {
        logd("setting storage root to $XDG_DATA_HOME({})", xdg_home);
        storage_dir = xdg_home;
    }

    auto dir = storage_dir /= STORAGE_DIR_NAME;
    logd("storage dir determined as {}", dir.string());
    determine_storage_dir_cache = dir;
    return determine_storage_dir_cache;
}

static bool is_directory_writable(const path &dir) {
    path test_file = dir / "test";
    std::ofstream ofs(test_file);
    if (ofs) {
        ofs.close();
        std::filesystem::remove(test_file);
        return true;
    }
    return false;
}

bool initialize_storage() {
    const path storage_dir = determine_storage_dir();

    if (!std::filesystem::exists(storage_dir)) {
        try {
            std::filesystem::create_directory(storage_dir);
            std::filesystem::permissions(storage_dir, STORAGE_PERMISSIONS, std::filesystem::perm_options::replace);
        } catch (std::filesystem::filesystem_error &e) {
            loge("failed to create directory '{}' with '{}'", storage_dir.string(), e.what());
            return false;
        }
    }

    if (!std::filesystem::is_directory(storage_dir)) {
        loge("storage_path {} exists, but is not a directory", storage_dir.string());
        return false;
    }

    if (!is_directory_writable(storage_dir)) {
        // TODO The dir not being writable might not actually be a problem if only reading
        loge("storage directory {} is not writable", storage_dir.string());
        return false;
    }

    auto inputs_dir = storage_dir / INPUTS_DIR_NAME;
    if (!std::filesystem::exists(inputs_dir)) {
        try {
            std::filesystem::create_directory(inputs_dir);
            std::filesystem::permissions(inputs_dir, STORAGE_PERMISSIONS, std::filesystem::perm_options::replace);
        } catch (std::filesystem::filesystem_error &e) {
            loge("failed to create inputs directory '{}' with '{}'", inputs_dir.string(), e.what());
            return false;
        }
    }

    return true;
}

bool empty_storage() {
    const path storage_dir = determine_storage_dir();
    loge("deleting everything from storage dir {}" , storage_dir.string());

    const auto inputs_dir = storage_dir / INPUTS_DIR_NAME;
    if (std::filesystem::exists(inputs_dir) && std::filesystem::is_directory(inputs_dir)) {
        logd("removing inputs directory {}", inputs_dir.string());
        for (const auto &file: std::filesystem::directory_iterator(inputs_dir)) {
            if (file.is_directory()) {
                logw("cannot remove file {} since it is a directory", file.path().string());
                continue;
            }
            logd("removing file {}", file.path().string());
            auto result = std::filesystem::remove(file.path());
            if (!result) {
                logw("could not remove file {}", file.path().string());
            }
        }

        if (!std::filesystem::is_empty(inputs_dir)) {
            logw("could not remove the inputs directory since it is not empty");
        } else {
            auto result = std::filesystem::remove(inputs_dir);
            if (!result) {
                logw("failed to remove directory {}", inputs_dir.string());
            } else {
                logd("removed directory {}", inputs_dir.string());
            }
        }
    }

    const auto profile_file = storage_dir / PROFILE_FILE_NAME;
    if (std::filesystem::exists(profile_file) && std::filesystem::is_regular_file(profile_file)) {
        logd("removing profile file {}", profile_file.string());
        auto result = std::filesystem::remove(profile_file.string());
        if (!result) {
            logw("could not remove profile file {}", profile_file.string());
        }
    }

    if (
        std::filesystem::exists(storage_dir) &&
        std::filesystem::is_directory(storage_dir) &&
        std::filesystem::is_empty(storage_dir)
    ) {
        logd("removing storage dir '{}'", storage_dir.string());
        if (std::filesystem::remove(storage_dir)) {
            logw("failed to remove storage dir '{}'", storage_dir.string());
        }
    } else {
        loge("could not remove storage dir '{}', not empty/not existing?", storage_dir.string());
        return false;
    }

    logd("done removing, no exceptions raised");
    return true;
}

bool has_profile_key() noexcept {
    const path profile_file = determine_storage_dir() / PROFILE_FILE_NAME;
    return std::filesystem::exists(profile_file) && !std::filesystem::is_empty(profile_file);
}

static std::string get_file_contents(const path &file_path) {
    std::ifstream in(file_path, std::ios::in);
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], (long)contents.size());
    in.close();

    return contents;
}

static bool write_to_file(const path &file_path, const std::string &contents) {
    std::ofstream out(file_path, std::ios::out);
    if (!out.is_open()) {
        logw("failed to read '{}' because is_open() failed!", file_path.string());
        return false;
    }
    out << contents;
    out.close();
    return true;
}

std::string get_profile_key() {
    const path profile_file = determine_storage_dir() / PROFILE_FILE_NAME;
    logd("getting key from file {}", profile_file.string());
    auto key = trim(get_file_contents(profile_file));
    logd("got a key that is {} characters long", key.length());
    return key;
}

static std::string get_file_name_for_puzzle_input(uint year, uint day) {
    return fmt::format("{}-{}.txt", year, day);
}

bool has_puzzle_input(uint year, uint day) noexcept {
    const path puzzle_input = determine_storage_dir() / INPUTS_DIR_NAME / get_file_name_for_puzzle_input(year, day);
    return std::filesystem::exists(puzzle_input) && !std::filesystem::is_empty(puzzle_input);
}

std::string get_puzzle_input(uint year, uint day) {
    const path puzzle_input_file = determine_storage_dir() / INPUTS_DIR_NAME / get_file_name_for_puzzle_input(year, day);
    logd("getting puzzle input from file {}", puzzle_input_file.string());
    auto puzzle_input = trim_only_newlines(get_file_contents(puzzle_input_file));
    logd("got an input that is {} characters long", puzzle_input.length());
    return puzzle_input;
}

bool store_key(const std::string &key) {
    const path profile_file = determine_storage_dir() / PROFILE_FILE_NAME;
    logd("storing key to {}", profile_file.string());
    auto success = write_to_file(profile_file, key);
    if (!success) {
        logw("failed to store key to {}", profile_file.string());
        return false;
    }
    std::filesystem::permissions(profile_file, KEY_PERMISSIONS, std::filesystem::perm_options::replace);
    return true;
}

bool store_puzzle_input(uint year, uint day, const std::string &input) {
    const path puzzle_input_file = determine_storage_dir() / INPUTS_DIR_NAME / get_file_name_for_puzzle_input(year, day);
    logd("storing puzzle input for {}-{} to {}", year, day, puzzle_input_file.string());
    return write_to_file(puzzle_input_file, input);
}

