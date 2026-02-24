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

constexpr NumberDataType
NumberMax(NumberWidth w, NumberSign s) {
    switch (w) {
    case Byte:
        return (s == Signed) ? 0x7F : 0xFF;
    case Word:
        return (s == Signed) ? 0x7FFF : 0xFFFF;
    case DWord:
        return (s == Signed) ? 0x7FFFFFFF : 0xFFFFFFFF;
    case QWord:
        return (s == Signed) ? 0x7FFFFFFFFFFFFFFF : 0xFFFFFFFFFFFFFFFF;
    default:
        return 0xFFFFFFFFFFFFFFFFull;
    }
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
    bool negative;
};

template <NumberBase base>
static constexpr size_t
MaxDigitsForType(void) {
    static_assert(base == Binary || base == Octal || base == Decimal ||
                      base == Hexadecimal,
                  "Invalid base");
    NumberDataType v = std::numeric_limits<NumberDataType>::max();
    size_t count = 0;
    do {
        ++count;
        v /= static_cast<NumberDataType>(base);
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
        if (value == 0) {
            DigitArray<N> digits;
            digits[0] = Digit0;
            digits.size = 1;
            return digits;
        }

        size_t count = 0;
        DigitArray<N> digits;
        if ((sign == Signed) && (base == Decimal)) { // signed
            auto widthMask = WidthMask(width);
            int64_t v = static_cast<int64_t>(value & widthMask);
            // check the sign bit
            bool negative;
            if (width < 64) {
                negative = ((v & BIT(width - 1)) != 0);
                if (negative) {
                    v = (v ^ widthMask) + 1;
                    v &= widthMask;
                }
            } else {
                negative = (v < 0);
                if (negative) {
                    v = -v;
                }
            }
            digits.negative = negative;
            for (size_t i = 0; i < N; ++i) {
                if (v == 0) {
                    break;
                }
                digits[i] = static_cast<Digit>(v % base);
                v /= base;
                count++;
            }
            digits.size = count;
        } else { // unsigned
            NumberDataType v = value;
            for (size_t i = 0; i < N; ++i) {
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
    NumberWidth width;
    NumberSign sign;
};

}; // namespace HexCalc
