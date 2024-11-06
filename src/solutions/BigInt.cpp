#include "BigInt.hpp"

#include <utility>

BigInt::BigInt(): value("0") {}

BigInt::BigInt(std::string value): value(std::move(value)) {
    // Remove leading zeros
    removeLeadingZeros();
}

void BigInt::removeLeadingZeros() {
    auto it = std::find_if(value.begin(), value.end(), [](char c) { return c != '0'; });
    value = (it == value.end()) ? "0" : std::string(it, value.end());
}

BigInt BigInt::operator+(const BigInt &addend) const {
    std::string result;
    const std::string& num1 = this->value;
    const std::string& num2 = addend.value;

    int carry = 0;
    int maxLength = (int)std::max(num1.length(), num2.length());

    // Pad shorter number with zeros
    std::string n1 = std::string(maxLength - num1.length(), '0') + num1;
    std::string n2 = std::string(maxLength - num2.length(), '0') + num2;

    // Perform addition from the last digit to the first
    for (int i = maxLength - 1; i >= 0; --i) {
        int digitSum = (n1[i] - '0') + (n2[i] - '0') + carry;
        carry = digitSum / 10;
        result += (char)(digitSum % 10) + '0';
    }
    if (carry) {
        result += (char)carry + '0';
    }

    // Reverse the result
    std::reverse(result.begin(), result.end());
    return BigInt(result);
}

BigInt BigInt::operator-(const BigInt &subtrahend) const  {
    // TODO: Handle negatives
    std::string result;
    const std::string& num1 = this->value;
    const std::string& num2 = subtrahend.value;

    int borrow = 0;

    // Pad shorter number with zeros
    std::string n1 = num1;
    std::string n2 = std::string(num1.length() - num2.length(), '0') + num2;

    // Perform subtraction from the last digit to the first
    for (int i = (int)n1.length() - 1; i >= 0; --i) {
        int digitDiff = (n1[i] - '0') - (n2[i] - '0') - borrow;
        if (digitDiff < 0) {
            digitDiff += 10; // borrow from the next digit
            borrow = 1;
        } else {
            borrow = 0;
        }
        result += (char)digitDiff + '0';
    }

    // Reverse the result and remove leading zeros
    std::reverse(result.begin(), result.end());
    return BigInt(result);
}

BigInt BigInt::operator*(const BigInt &multiplier) const {
    const std::string& num1 = this->value;
    const std::string& num2 = multiplier.value;

    // Result can be at most len(num1) + len(num2)
    std::string result(num1.length() + num2.length(), '0');

    for (int i = (int)num1.length() - 1; i >= 0; --i) {
        int n1 = num1[i] - '0';
        int carry = 0;

        for (int j = (int)num2.length() - 1; j >= 0; --j) {
            int n2 = num2[j] - '0';
            int sum = (result[i + j + 1] - '0') + n1 * n2 + carry;
            carry = sum / 10; // Calculate carry for next digit
            result[i + j + 1] = (char)(sum % 10) + '0'; // Store the current digit
        }

        // Add any remaining carry to the next position
        result[i] += (char)carry;
    }

    return BigInt(result);
}

std::ostream &operator<<(std::ostream &os, const BigInt &bigInt) {
    os << bigInt.value;
    return os;
}

std::string BigInt::to_string() {
    return value;
}

bool BigInt::operator==(const BigInt &other) const {
    return this->value == other.value;
}

bool BigInt::operator<(const BigInt& other) const {
    // Compare lengths first
    if (this->value.length() < other.value.length()) {
        return true; // this is smaller
    }
    if (this->value.length() > other.value.length()) {
        return false; // this is larger
    }
    // If lengths are equal, compare digit by digit
    return this->value < other.value; // lexicographical comparison
}


bool BigInt::operator>(const BigInt &other) const {
    return *this != other && !(*this < other);
}

BigInt& BigInt::operator=(const BigInt& other) {
    if (this != &other) { // Self-assignment check
        value = other.value;  // Copy the value from the other BigInt
        removeLeadingZeros();  // Remove leading zeros if needed
    }
    return *this;
}

BigInt BigInt::operator%(const BigInt &divisor) const {
    if (divisor.value == "0") {
        throw std::invalid_argument("Modulo by zero is undefined.");
    }

    std::string result;
    int remainder = 0;

    for (char digit : value) {
        remainder = (remainder * 10 + (digit - '0')) % std::stoi(divisor.value);
    }

    return BigInt(std::to_string(remainder));
}

BigInt BigInt::operator/(const BigInt& divisor) const {
    if (divisor.value == "0") {
        throw std::invalid_argument("Division by zero is undefined.");
    }

    BigInt dividend = *this;
    BigInt quotient("0");
    BigInt remainder("0");

    // Process each digit of the dividend
    for (char i : dividend.value) {
        // Bring down the next digit
        remainder.value += i;

//        // Remove leading zeros from remainder
//        while (remainder.value.length() > 1 && remainder.value[0] == '0') {
//            remainder.value.erase(remainder.value.begin());
//        }
        remainder.removeLeadingZeros();

        // Find how many times the divisor fits into the current remainder
        int count = 0;
        while (remainder >= divisor) {
            remainder = remainder - divisor; // Subtract divisor from remainder
            count++;
        }

        quotient.value += std::to_string(count);
    }

    // Clean the quotient from leading zeros
    quotient.removeLeadingZeros();
    return quotient;
}

bool BigInt::operator<=(const BigInt &other) const {
    return *this == other || *this < other;
}

bool BigInt::operator>=(const BigInt &other) const {
    return *this == other || *this > other;
}
