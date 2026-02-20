/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "test_config.h"

#include "config.cc"
#include "number.h"

TEST_GROUP(Number){};

TEST(Number, TestQwordHexadecimal) {
    constexpr uint64_t testValue = 0x0d000721;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<HexCalc::Hexadecimal>();
    CHECK_EQUAL(7, res.size);

    for (size_t i = 0; i < res.size; ++i) {
        CHECK_EQUAL(static_cast<HexCalc::Digit>(testValue >> (4 * i)) & 0xF,
                    res[i]);
    }
}

TEST(Number, TestDwordDecimal) {
    constexpr uint64_t testValue = 1234567890;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<HexCalc::Decimal>();
    CHECK_EQUAL(10, res.size);
}

TEST(Number, TestWordOctal) {
    constexpr uint64_t testValue = 01234567;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<HexCalc::Octal>();
    CHECK_EQUAL(7, res.size);
}

TEST(Number, TestByteBinary) {
    constexpr uint64_t testValue = 0b10010011;
    HexCalc::Number num(testValue);
    auto res = num.Transcode<HexCalc::Binary>();
    CHECK_EQUAL(8, res.size);
}

TEST(Number, TestNegativeDecimal) {
    constexpr int64_t testValue = -1234567890;
    HexCalc::Number num(testValue, HexCalc::Signed);
    auto res = num.Transcode<HexCalc::Decimal>();
    CHECK(res.isNegative);
    CHECK_EQUAL(10, res.size);
}

int
main(int ac, char **av) {
    return CommandLineTestRunner::RunAllTests(ac, av);
}
