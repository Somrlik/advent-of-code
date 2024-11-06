#ifndef AOC_BIGINT_HPP
#define AOC_BIGINT_HPP

#include <string>
#include <iostream>
#include "fmt/format.h"

// These macros generate arithmetic operators between instances and build-in numeric types
#define CONCATENATE(x, y) x##y
#define ARITHMETIC_OPERATOR(op) \
    BigInt operator op(const BigInt& addend) const; \
    template<typename T> BigInt operator op(T other) const { \
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric"); \
        return *this op BigInt(std::to_string(other)); \
    }
#define ARITHMETIC_ASSIGMENT_OPERATOR(op) \
    BigInt& operator CONCATENATE(op, =)(const BigInt& other) { \
        BigInt result = *this op other; \
        this->value = result.value; \
        return *this; \
    } \
    template<typename T> \
    BigInt& operator CONCATENATE(op, =)(T other) { \
        return *this CONCATENATE(op, =) BigInt(std::to_string(other)); \
    }
#define COMPARISON_OPERATOR(op) \
    bool operator op(const BigInt& other) const; \
    template<typename T> bool operator op(T other) const { \
        static_assert(std::is_arithmetic<T>::value, "Value must be numeric"); \
        return *this op BigInt(std::to_string(other)); \
    }


class BigInt {
public:
    BigInt();
    explicit BigInt(std::string value);
    explicit BigInt(int number): value(std::to_string(number)) {}
    explicit BigInt(long number): value(std::to_string(number)) {}

    // Arithmetic
    ARITHMETIC_OPERATOR(+); ARITHMETIC_ASSIGMENT_OPERATOR(+);
    ARITHMETIC_OPERATOR(-); ARITHMETIC_ASSIGMENT_OPERATOR(-);
    ARITHMETIC_OPERATOR(*); ARITHMETIC_ASSIGMENT_OPERATOR(*);
    ARITHMETIC_OPERATOR(/); ARITHMETIC_ASSIGMENT_OPERATOR(/);
    ARITHMETIC_OPERATOR(%); ARITHMETIC_ASSIGMENT_OPERATOR(%);

    // Comparison
    COMPARISON_OPERATOR(==);
    COMPARISON_OPERATOR(<);
    COMPARISON_OPERATOR(<=);
    COMPARISON_OPERATOR(>);
    COMPARISON_OPERATOR(>=);

    // Assignment
    BigInt& operator=(const BigInt& other);

    // Output operator
    friend std::ostream& operator<<(std::ostream& os, const BigInt& bigInt);
    std::string to_string();
    friend std::string inline format_as(const BigInt& f) {
        return f.value;
    }
private:
    std::string value;

    void removeLeadingZeros();
};

#endif
