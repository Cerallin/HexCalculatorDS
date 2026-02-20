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

template <size_t N>
struct DigitArray {
    constexpr DigitArray(void) : digits{}, size(0), isNegative(false) {}

    auto &
    operator[](size_t index) {
        return digits[index];
    }

    const auto &
    operator[](size_t index) const {
        return digits[index];
    }

    Digit digits[N];
    size_t size;
    bool isNegative;
};

using NumberDataType = uint64_t;
static constexpr NumberDataType NumberZero = NumberDataType(0);

/**
 * @brief A number with a specified width and sign.
 *
 * @tparam width The width of the number in bits (8, 16, 32, or 64).
 * @tparam sign The sign of the number (signed or unsigned).
 */
class Number {
  public:
    constexpr Number(void) : Number(0) {}
    constexpr explicit Number(uint64_t v, NumberSign sign = Unsigned)
        : value(v), sign(sign) {}

    constexpr uint64_t
    Raw() const noexcept {
        return value;
    }

    static constexpr size_t MaxBinDigits = 64;
    static constexpr size_t MaxOctDigits = 22;
    static constexpr size_t MaxDecDigits = 22;
    static constexpr size_t MaxHexDigits = 22;

    /**
     * @brief Get max digits for the number based on its width and sign.
     *
     * @tparam base
     * @return constexpr size_t
     */
    template <NumberBase base>
    static constexpr size_t
    MaxDigits(void) {
        static_assert(base == Binary || base == Octal || base == Decimal ||
                          base == Hexadecimal,
                      "Invalid base");
        // consider 64 for binary and 22 for other bases
        return (base == Binary)
                   ? MaxBinDigits
                   : std::max(MaxOctDigits,
                              std::max(MaxDecDigits, MaxHexDigits));
    }

    template <NumberBase base>
    auto
    Transcode() const {
        if (value == 0) {
            DigitArray<MaxDigits<base>()> digits;
            digits[0] = Digit0;
            digits.size = 1;
            return digits;
        }

        size_t count = 0;
        constexpr auto maxDigits = MaxDigits<base>();
        DigitArray<maxDigits> digits;
        if (sign == Signed) { // signed
            int64_t v = static_cast<int64_t>(value);
            digits.isNegative = (v < 0);
            if (v < 0) {
                v = -v;
            }
            for (size_t i = 0; i < maxDigits; ++i) {
                if (v == 0) {
                    break;
                }
                digits[i] = static_cast<Digit>(v % base);
                v /= base;
                count++;
            }
            digits.size = count;
        } else { // unsigned
            uint64_t v = value;
            for (size_t i = 0; i < maxDigits; ++i) {
                if (v == 0) {
                    break;
                }
                digits[i] = static_cast<Digit>(v % base);
                v /= base;
                count++;
            }
            digits.size = count;
        }
        return digits;
    }

  private:
    NumberDataType value{};
    NumberSign sign;
};
}; // namespace HexCalc
