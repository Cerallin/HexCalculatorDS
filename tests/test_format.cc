/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "test_config.h"

#include "format.h"

namespace {

bool
GlyphEquals(const HexCalc::Glyph &lhs, const HexCalc::Glyph &rhs) {
    return lhs.Upper() == rhs.Upper() && lhs.Lower() == rhs.Lower() &&
           lhs.UpperHFlip() == rhs.UpperHFlip() &&
           lhs.UpperVFlip() == rhs.UpperVFlip() &&
           lhs.LowerHFlip() == rhs.LowerHFlip() &&
           lhs.LowerVFlip() == rhs.LowerVFlip() &&
           lhs.UnderBaseline() == rhs.UnderBaseline();
}

void
CheckGlyphFont(const HexCalc::Glyph &glyph, HexCalc::FontType font) {
    CHECK_TRUE(GlyphEquals(glyph, HexCalc::Glyph(font)));
}

constexpr bool
TestFormatSizeConstexpr(void) {
    return HexCalc::GlyphFormatSize<HexCalc::Hexadecimal>(16) == 19 &&
           HexCalc::GlyphFormatSize<HexCalc::Decimal>(10) == 14 &&
           HexCalc::GlyphFormatSize<HexCalc::Octal>(8) == 11 &&
           HexCalc::GlyphFormatSize<HexCalc::Binary>(16) == 19;
}

static_assert(TestFormatSizeConstexpr(),
              "GlyphFormatSize should remain constexpr-evaluable");

} // namespace

TEST_GROUP(Format){};

TEST(Format, GlyphArrayBuildsInDisplayOrderByDefault) {
    HexCalc::DigitArray<8> digits;
    digits[0] = HexCalc::DigitA;
    digits[1] = HexCalc::DigitB;
    digits[2] = HexCalc::DigitC;
    digits.size = 3;

    HexCalc::GlyphArray6x8<8> glyphs(digits);

    CHECK_EQUAL(3, glyphs.Size());
    CheckGlyphFont(glyphs[0], HexCalc::Font6x8C);
    CheckGlyphFont(glyphs[1], HexCalc::Font6x8B);
    CheckGlyphFont(glyphs[2], HexCalc::Font6x8A);
}

TEST(Format, GlyphArrayRespectsReverseOrder) {
    HexCalc::DigitArray<8> digits;
    digits[0] = HexCalc::DigitA;
    digits[1] = HexCalc::DigitB;
    digits[2] = HexCalc::DigitC;
    digits.size = 3;

    HexCalc::GlyphArray6x8<8> glyphs(digits, true);

    CHECK_EQUAL(3, glyphs.Size());
    CheckGlyphFont(glyphs[0], HexCalc::Font6x8A);
    CheckGlyphFont(glyphs[1], HexCalc::Font6x8B);
    CheckGlyphFont(glyphs[2], HexCalc::Font6x8C);
}

TEST(Format, DecimalFormattingAddsMinusAndCommaSeparators) {
    HexCalc::DigitArray<HexCalc::Number::MaxDecDigits> digits;
    // 1,234,567 (LSD first)
    digits[0] = HexCalc::Digit7;
    digits[1] = HexCalc::Digit6;
    digits[2] = HexCalc::Digit5;
    digits[3] = HexCalc::Digit4;
    digits[4] = HexCalc::Digit3;
    digits[5] = HexCalc::Digit2;
    digits[6] = HexCalc::Digit1;
    digits.size = 7;
    digits.negative = true;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Decimal, 6, 8>(digits);

    CHECK_EQUAL(10, formatted.Size());
    CheckGlyphFont(formatted[0], HexCalc::Font6x8Minus);
    CheckGlyphFont(formatted[1], HexCalc::Font6x8One);
    CheckGlyphFont(formatted[2], HexCalc::Font6x8Comma);
    CheckGlyphFont(formatted[3], HexCalc::Font6x8Two);
    CheckGlyphFont(formatted[4], HexCalc::Font6x8Three);
    CheckGlyphFont(formatted[5], HexCalc::Font6x8Four);
    CheckGlyphFont(formatted[6], HexCalc::Font6x8Comma);
    CheckGlyphFont(formatted[7], HexCalc::Font6x8Five);
    CheckGlyphFont(formatted[8], HexCalc::Font6x8Six);
    CheckGlyphFont(formatted[9], HexCalc::Font6x8Seven);
}

TEST(Format, HexFormattingPadsToGroupSizeWithoutVisibleSeparator) {
    HexCalc::DigitArray<HexCalc::Number::MaxHexDigits> digits;
    digits[0] = HexCalc::DigitB;
    digits[1] = HexCalc::DigitA;
    digits.size = 2;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Hexadecimal, 6, 8>(digits);

    CHECK_EQUAL(4, formatted.Size());
    CheckGlyphFont(formatted[0], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[1], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[2], HexCalc::Font6x8A);
    CheckGlyphFont(formatted[3], HexCalc::Font6x8B);
}

TEST(Format, BinaryFormattingPadsTo16AndInsertsGroupSeparators) {
    HexCalc::DigitArray<HexCalc::Number::MaxBinDigits> digits;
    digits[0] = HexCalc::Digit1;
    digits.size = 1;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Binary, 6, 8>(digits);

    CHECK_EQUAL(19, formatted.Size());
    CheckGlyphFont(formatted[0], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[4], HexCalc::FontEmpty);
    CheckGlyphFont(formatted[5], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[9], HexCalc::FontEmpty);
    CheckGlyphFont(formatted[10], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[14], HexCalc::FontEmpty);
    CheckGlyphFont(formatted[18], HexCalc::Font6x8One);
}

TEST(Format, Decimal8x8Uses8x8GlyphTraits) {
    HexCalc::DigitArray<HexCalc::Number::MaxDecDigits> digits;
    digits[0] = HexCalc::Digit2;
    digits[1] = HexCalc::Digit4;
    digits.size = 2;
    digits.negative = true;

    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Decimal, 8, 8>(digits);

    CHECK_EQUAL(3, formatted.Size());
    CheckGlyphFont(formatted[0], HexCalc::Font8x8Minus);
    CheckGlyphFont(formatted[1], HexCalc::Font8x8Four);
    CheckGlyphFont(formatted[2], HexCalc::Font8x8Two);
}

TEST(Format, VisitFormattedGlyphArrayDispatchesByBase) {
    HexCalc::DigitArray<HexCalc::Number::MaxHexDigits> digits;
    digits[0] = HexCalc::DigitA;
    digits[1] = HexCalc::Digit1;
    digits.size = 2;

    auto getSize = [&](HexCalc::NumberBase base) {
        return HexCalc::VisitFormattedGlyphArray<6, 8>(
            base, digits, false,
            [](const auto &arr) { return static_cast<int>(arr.Size()); });
    };

    CHECK_EQUAL(4, getSize(HexCalc::Hexadecimal));
    CHECK_EQUAL(2, getSize(HexCalc::Decimal));
    CHECK_EQUAL(3, getSize(HexCalc::Octal));
    CHECK_EQUAL(19, getSize(HexCalc::Binary));
}

TEST(Format, HeaderGlyphArrayHasThreeEntries) {
    HexCalc::HeaderGlyphArray6x8 header(HexCalc::Glyph(HexCalc::Font6x8HH),
                                        HexCalc::Glyph(HexCalc::Font6x8EH),
                                        HexCalc::Glyph(HexCalc::Font6x8XH));

    CHECK_EQUAL(3, header.Size());
    CheckGlyphFont(header[0], HexCalc::Font6x8HH);
    CheckGlyphFont(header[1], HexCalc::Font6x8EH);
    CheckGlyphFont(header[2], HexCalc::Font6x8XH);
}

TEST(Format, SignedQwordNegative25OctalKeepsTrailing47) {
    HexCalc::Number number(static_cast<HexCalc::NumberDataType>(-25),
                           HexCalc::QWord, HexCalc::Signed);
    auto digits =
        number.Transcode<HexCalc::Number::MaxOctDigits>(HexCalc::Octal);
    auto formatted =
        HexCalc::MakeFormattedGlyphArray<HexCalc::Octal, 6, 8>(digits);

    // 1777777777777777777747(8) -> 001 777 777 777 777 777 777 747
    CHECK_EQUAL(22, digits.size);
    CHECK_EQUAL(31, formatted.Size());

    CheckGlyphFont(formatted[0], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[1], HexCalc::Font6x8Zero);
    CheckGlyphFont(formatted[2], HexCalc::Font6x8One);

    CheckGlyphFont(formatted[28], HexCalc::Font6x8Seven);
    CheckGlyphFont(formatted[29], HexCalc::Font6x8Four);
    CheckGlyphFont(formatted[30], HexCalc::Font6x8Seven);
}

int
main(int ac, char **av) {
    return CommandLineTestRunner::RunAllTests(ac, av);
}
