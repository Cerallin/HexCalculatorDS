/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

namespace HexCalc {

enum NumberBase : uint8_t {
    Binary = 2,
    Octal = 8,
    Decimal = 10,
    Hexadecimal = 16,
};

enum NumberWidth : uint8_t {
    QWord = 64,
    DWord = 32,
    Word = 16,
    Byte = 8,
};

enum NumberSign : uint8_t {
    Signed,
    Unsigned,
};

enum Digit : int8_t {
    DigitEOS = -1,
    Digit0 = 0,
    Digit1 = 1,
    Digit2 = 2,
    Digit3 = 3,
    Digit4 = 4,
    Digit5 = 5,
    Digit6 = 6,
    Digit7 = 7,
    Digit8 = 8,
    Digit9 = 9,
    DigitA = 10,
    DigitB = 11,
    DigitC = 12,
    DigitD = 13,
    DigitE = 14,
    DigitF = 15,
};

template <int N>
struct DigitArray {
    Digit digits[N];
    constexpr DigitArray() : digits{} {
        for (int i = 0; i < N; ++i) {
            digits[i] = DigitEOS;
        }
    }
};

using NumberDataType = uint64_t;
static constexpr NumberDataType NumberZero = NumberDataType(0);

/**
 * @brief A number with a specified base and width.
 *
 * @tparam base The base of the number (2, 8, 10, or 16).
 * @tparam width The width of the number in bits (8, 16, 32, or 64).
 */
template <NumberBase base>
class Number {
  public:
    constexpr Number(void) : Number(0) {}
    constexpr explicit Number(uint64_t v) : value(v) {
        static_assert(base == Binary || base == Octal || base == Decimal ||
                          base == Hexadecimal,
                      "Invalid base");
    }

    template <NumberBase otherBase>
    constexpr Number(const Number<otherBase> &other) noexcept
        : value(other.Raw()) {}

    constexpr uint64_t
    Raw() const noexcept {
        return value;
    }

  private:
    NumberDataType value{};
};

}; // namespace HexCalc
