/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "structure.h"

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

enum NumberShiftMode : uint8_t {
    ArithmeticMode,
    CircularMode,
    LogicalMode,
    MAX_SHIFT_MODE_COUNT,
};

using NumberDataType = uint64_t;
static constexpr NumberDataType NumberZero = NumberDataType(0);

/**
 * @brief Get the width mask for a given number width.
 *
 * @param w The number width.
 * @return constexpr NumberDataType The width mask.
 */
constexpr NumberDataType
WidthMask(NumberWidth w) {
    switch (w) {
    case Byte:
        return 0xFFull;
    case Word:
        return 0xFFFFull;
    case DWord:
        return 0xFFFFFFFFull;
    case QWord:
        return 0xFFFFFFFFFFFFFFFFull;
    default:
        return 0xFFFFFFFFFFFFFFFFull;
    }
}

/**
 * @brief Whether appending @p digit in @p base would exceed the current width.
 */
constexpr bool
WouldOverflowDigit(NumberDataType current, NumberDataType digit,
                   NumberBase base, NumberWidth width) {
    const NumberDataType maxValue = WidthMask(width);
    return current > (maxValue - digit) / static_cast<NumberDataType>(base);
}

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
    constexpr DigitArray(void) : digits{}, size(0), negative(false) {}

    constexpr auto &
    operator[](size_t index) {
        return digits[index];
    }

    constexpr const auto &
    operator[](size_t index) const {
        return digits[index];
    }

    Digit digits[N];
    size_t size;
    bool negative;
};

template <NumberBase Base>
static constexpr size_t
MaxDigitsForType(void) {
    static_assert(Base == Binary || Base == Octal || Base == Decimal ||
                      Base == Hexadecimal,
                  "Invalid base");
    NumberDataType v = std::numeric_limits<NumberDataType>::max();
    size_t count = 0;
    do {
        ++count;
        v /= static_cast<NumberDataType>(Base);
    } while (v != 0);
    return count;
}

/**
 * @brief A number with a specified width and sign.
 *
 * @tparam width The width of the number in bits (8, 16, 32, or 64).
 * @tparam sign The sign of the number (signed or unsigned).
 */
class Number {
  public:
    constexpr Number(void) : Number(0) {}
    constexpr explicit Number(NumberDataType v, NumberWidth width = QWord,
                              NumberSign sign = Unsigned)
        : value(v), width(width), sign(sign) {}

    static constexpr size_t MaxBinDigits = MaxDigitsForType<Binary>() / 4;
    static constexpr size_t MaxOctDigits = MaxDigitsForType<Octal>();
    static constexpr size_t MaxDecDigits = MaxDigitsForType<Decimal>();
    static constexpr size_t MaxHexDigits = MaxDigitsForType<Hexadecimal>();

    template <size_t N>
    auto
    Transcode(NumberBase base) const {
        NumberDataType v = value & WidthMask(width);
        if (v == 0) {
            DigitArray<N> digits;
            digits[0] = Digit0;
            digits.size = 1;
            return digits;
        }

        size_t count = 0;
        DigitArray<N> digits;
        if ((sign == Signed) && (base == Decimal)) {
            // Use unsigned two's-complement abs so INT*_MIN (e.g.
            // 0x8000000000000000) does not invoke signed negation UB.
            auto widthMask = WidthMask(width);
            const bool negative = (v & (NumberDataType(1) << (width - 1))) != 0;
            if (negative) {
                v = ((v ^ widthMask) + 1) & widthMask;
            }
            digits.negative = negative;
        }
        for (size_t i = 0; i < N; ++i) {
            if (v == 0) {
                break;
            }
            digits[i] = static_cast<Digit>(v % base);
            v /= base;
            count++;
        }
        digits.size = count;
        return digits;
    }

  private:
    NumberDataType value{};
    NumberWidth width;
    NumberSign sign;
};

}; // namespace HexCalc
