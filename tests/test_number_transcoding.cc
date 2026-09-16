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

TEST(Number, TestSignedQWordMinDecimal) {
    // 0x8000000000000000 == INT64_MIN == -9223372036854775808
    constexpr uint64_t testValue = 0x8000000000000000ull;
    HexCalc::Number num(testValue, HexCalc::QWord, HexCalc::Signed);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);

    CHECK(res.negative);
    CHECK_EQUAL(19, res.size);

    constexpr const char *expected = "9223372036854775808";
    for (size_t i = 0; i < res.size; ++i) {
        CHECK_EQUAL(
            static_cast<HexCalc::Digit>(expected[res.size - 1 - i] - '0'),
            res[i]);
    }
}

TEST(Number, TestSignedDWordMinDecimal) {
    constexpr uint64_t testValue = 0x80000000ull;
    HexCalc::Number num(testValue, HexCalc::DWord, HexCalc::Signed);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);

    CHECK(res.negative);
    CHECK_EQUAL(10, res.size);

    constexpr const char *expected = "2147483648";
    for (size_t i = 0; i < res.size; ++i) {
        CHECK_EQUAL(
            static_cast<HexCalc::Digit>(expected[res.size - 1 - i] - '0'),
            res[i]);
    }
}

TEST(Number, TestSignedByteMinDecimal) {
    constexpr uint64_t testValue = 0x80ull;
    HexCalc::Number num(testValue, HexCalc::Byte, HexCalc::Signed);
    auto res = num.Transcode<MaxDigits>(HexCalc::Decimal);

    CHECK(res.negative);
    CHECK_EQUAL(3, res.size);
    CHECK_EQUAL(HexCalc::Digit8, res[0]);
    CHECK_EQUAL(HexCalc::Digit2, res[1]);
    CHECK_EQUAL(HexCalc::Digit1, res[2]);
}

TEST(Number, TestHexInputAllowsSignedQWordMinPattern) {
    // Typing 0x800000000000000 + '0' must be accepted under QWord width.
    CHECK_FALSE(HexCalc::WouldOverflowDigit(
        0x800000000000000ull, 0, HexCalc::Hexadecimal, HexCalc::QWord));
    // Once at 0x8000000000000000, another digit must be rejected.
    CHECK(HexCalc::WouldOverflowDigit(0x8000000000000000ull, 0,
                                      HexCalc::Hexadecimal, HexCalc::QWord));
}

TEST(Number, TestHexInputRejectsBeyondWidth) {
    CHECK(HexCalc::WouldOverflowDigit(0xFFull, 0, HexCalc::Hexadecimal,
                                      HexCalc::Byte));
    CHECK_FALSE(HexCalc::WouldOverflowDigit(0xFull, 0xF, HexCalc::Hexadecimal,
                                            HexCalc::Byte));
}

int
main(int ac, char **av) {
    return CommandLineTestRunner::RunAllTests(ac, av);
}
