#include <gtest/gtest.h>

#include "../src/solutions/BigInt.hpp"

TEST(BigInt, Equivalency) {
    std::vector<std::tuple<std::string, std::string>> tests = {
        {"1", "1"},
        {"10", "10"},
        {"123456789", "123456789"},
        {"1234567890123456789012345678901234567890", "1234567890123456789012345678901234567890"},
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)}, BigInt{std::get<1>(test)});
    }

    ASSERT_TRUE(BigInt{"3"} == 3);
    ASSERT_TRUE(BigInt{"3"} == 3l);
    ASSERT_TRUE(BigInt{"3"} == 3ul);
}

TEST(BigInt, Addition) {
    std::vector<std::tuple<std::string, std::string, std::string>> tests = {
        {"1", "1", "2"},
        {"10", "10", "20"},
        {"1234567890123456789012345678901234567890", "1234567890123456789012345678901234567890", "2469135780246913578024691357802469135780"}
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)} + BigInt{std::get<1>(test)}, BigInt{std::get<2>(test)});
    }
}

TEST(BigInt, Subtraction) {
    std::vector<std::tuple<std::string, std::string, std::string>> tests = {
            {"1", "1", "0"},
            {"10", "10", "0"},
            {"20", "10", "10"},
            {"2469135780246913578024691357802469135780", "1234567890123456789012345678901234567890", "1234567890123456789012345678901234567890"}
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)} - BigInt{std::get<1>(test)}, BigInt{std::get<2>(test)});
    }
}

TEST(BigInt, Multiplication) {
    std::vector<std::tuple<std::string, std::string, std::string>> tests = {
            {"1", "1", "1"},
            {"10", "10", "100"},
            {"1234567890123456789012345678901234567890", "1234567890123456789012345678901234567890", "1524157875323883675049535156256668194500533455762536198787501905199875019052100"}
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)} * BigInt{std::get<1>(test)}, BigInt{std::get<2>(test)});
    }
}

TEST(BigInt, Modulo) {
    std::vector<std::tuple<std::string, std::string, std::string>> tests = {
            {"1234567890", "2", "0"},
            {"1234567890", "3", "0"},
            {"1234567890", "4", "2"},
            {"1234567890", "5", "0"},
            {"1234567890", "6", "0"},
            {"1234567890", "7", "3"},
            {"1234567890", "8", "2"},
            {"1234567890", "9", "0"},
            // ---------
            {"987654321", "2", "1"},
            {"987654321", "3", "0"},
            {"987654321", "4", "1"},
            {"987654321", "5", "1"},
            {"987654321", "6", "3"},
            {"987654321", "7", "3"},
            {"987654321", "8", "1"},
            {"987654321", "9", "0"},
            // ---------
            {"1234567890123456789012345678901234567890", "2", "0"},
            {"1234567890123456789012345678901234567890", "3", "0"},
            {"1234567890123456789012345678901234567890", "4", "2"},
            {"1234567890123456789012345678901234567890", "5", "0"},
            {"1234567890123456789012345678901234567890", "6", "0"},
            {"1234567890123456789012345678901234567890", "7", "3"},
            {"1234567890123456789012345678901234567890", "8", "2"},
            {"1234567890123456789012345678901234567890", "9", "0"},
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)} % BigInt{std::get<1>(test)}, BigInt{std::get<2>(test)});
    }
}

TEST(BigInt, Division) {
    std::vector<std::tuple<std::string, std::string, std::string>> tests = {
            {"1", "1", "1"},
            {"10", "10", "1"},
            {"100000000000000000000", "3", "33333333333333333333"}
    };

    for (const auto &test: tests) {
        ASSERT_EQ(BigInt{std::get<0>(test)} / BigInt{std::get<1>(test)}, BigInt{std::get<2>(test)});
    }
}
