#ifndef ADVENT_OF_CODE_CACHE_H
#define ADVENT_OF_CODE_CACHE_H

#include <filesystem>
#include <string>

std::filesystem::path determine_storage_dir();

bool initialize_storage();

bool empty_storage();

bool has_profile_key() noexcept;
bool store_key(const std::string&);
std::string get_profile_key();

bool has_puzzle_input(uint year, uint day) noexcept;
bool store_puzzle_input(uint year, uint day, const std::string&);
std::string get_puzzle_input(uint year, uint day);

#endif
