/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "test_config.h"

#include "config.cc"
#include "format.h"
#include "number.h"

static constexpr size_t MaxDigits = 64;

namespace {

constexpr bool
TestHexFormattingConstexpr(void) {
    HexCalc::DigitArray<HexCalc::Number::MaxHexDigits> digits;
    digits[0] = HexCalc::DigitA;
    digits[1] = HexCalc::DigitB;
    digits.size = 2;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Hexadecimal, 6, 8>(digits);

    return formatted.Size() == 4;
}

constexpr bool
TestDecimalFormattingConstexpr(void) {
    HexCalc::DigitArray<HexCalc::Number::MaxDecDigits> digits;
    digits[0] = HexCalc::Digit5;
    digits.size = 1;
    digits.negative = true;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Decimal, 6, 8>(digits);

    return formatted.Size() == 2;
}

constexpr bool
TestBinaryFormattingConstexpr(void) {
    HexCalc::DigitArray<HexCalc::Number::MaxBinDigits> digits;
    digits[0] = HexCalc::Digit1;
    digits.size = 1;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Binary, 6, 8>(digits);

    return formatted.Size() == 19;
}

static_assert(TestHexFormattingConstexpr(),
              "hex formatting should stay constexpr-evaluable");
static_assert(TestDecimalFormattingConstexpr(),
              "decimal formatting should stay constexpr-evaluable");
static_assert(TestBinaryFormattingConstexpr(),
              "binary formatting should use the shared constexpr pipeline");

} // namespace

TEST_GROUP(Number){};

TEST(Number, TestZero) {
    HexCalc::Number num(0);
    auto res = num.Transcode<MaxDigits>(HexCalc::Hexadecimal);
    CHECK_EQUAL(1, res.size);
}

TEST(Number, TestQwordHexadecimal) {
    constexpr uint64_t testValue = 0x0d000721;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<MaxDigits>(HexCalc::Hexadecimal);
    CHECK_EQUAL(7, res.size);

    for (size_t i = 0; i < res.size; ++i) {
        CHECK_EQUAL(static_cast<HexCalc::Digit>((testValue >> (4 * i)) & 0xF),
                    res[i]);
    }
}

TEST(Number, TestDwordDecimal) {
    constexpr uint64_t testValue = 1234567890;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);
    CHECK_EQUAL(10, res.size);
}

TEST(Number, TestWordOctal) {
    constexpr uint64_t testValue = 01234567;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<MaxDigits>(HexCalc::Octal);
    CHECK_EQUAL(7, res.size);
}

TEST(Number, TestByteBinary) {
    constexpr uint64_t testValue = 0b10010011;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<MaxDigits>(HexCalc::Binary);
    CHECK_EQUAL(8, res.size);
}

TEST(Number, TestNegativeDecimal) {
    constexpr int64_t testValue = -1234567890;
    HexCalc::Number num(testValue, HexCalc::QWord, HexCalc::Signed);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);
    CHECK(res.negative);
    CHECK_EQUAL(10, res.size);
}

TEST(Number, TestNegativeWord) {
    constexpr int64_t testValue = -12345;
    HexCalc::Number num(testValue, HexCalc::Word, HexCalc::Signed);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);
    CHECK(res.negative);
    CHECK_EQUAL(5, res.size);
}

int
main(int ac, char **av) {
    return CommandLineTestRunner::RunAllTests(ac, av);
}
