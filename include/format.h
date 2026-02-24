/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "font.h"
#include "number.h"

namespace HexCalc {

/**
 * @brief
 *
 * @tparam W Tile width
 * @tparam H Tile height
 * @tparam N Maximum number of glyphs
 */
template <int W, int H, size_t N>
class GlyphArray {
  protected:
    static constexpr FontType fontZero = (W == 6 && H == 8) ? Font6x8Zero
                                         : (W == 8 && H == 8)
                                             ? Font8x8Zero
                                             : /* default */ Font6x8Zero;

    static constexpr FontType fontMinus = (W == 6 && H == 8) ? Font6x8Minus
                                          : (W == 8 && H == 8)
                                              ? Font8x8Minus
                                              : /* default */ Font6x8Minus;

  public:
    static constexpr int CharWidth = W;
    static constexpr int CharHeight = H;

    constexpr GlyphArray(void) : glyphs{}, size(0), negative(false) {}
    constexpr GlyphArray(DigitArray<N> digits, bool reverse = false)
        : glyphs{}, size(0), negative(digits.negative) {
        if (reverse) {
            for (size_t i = 0; i < digits.size; i++) {
                this->Insert(
                    Glyph(static_cast<FontType>(digits[i] + fontZero)));
            }
        } else {
            for (size_t i = 0; i < digits.size; i++) {
                this->Insert(Glyph(static_cast<FontType>(
                    digits[digits.size - 1 - i] + fontZero)));
            }
        }
    }

    bool
    Insert(const Glyph &glyph) {
        if (size >= N) {
            return false;
        }

        glyphs[size++] = glyph;

        return true;
    }

    size_t
    Size(void) const {
        return size;
    }

    using iterator = Glyph *;
    using const_iterator = const Glyph *;

    const Glyph &
    operator[](size_t index) const {
        return glyphs[index];
    }

    iterator
    begin() {
        return glyphs;
    }
    iterator
    end() {
        return glyphs + size;
    }

    const_iterator
    begin() const {
        return glyphs;
    }
    const_iterator
    end() const {
        return glyphs + size;
    }

    const_iterator
    cbegin() const {
        return glyphs;
    }
    const_iterator
    cend() const {
        return glyphs + size;
    }

    bool
    Negative(void) const {
        return negative;
    }

  protected:
    Glyph glyphs[N];
    size_t size;

  private:
    bool negative;
};

template <size_t N>
using GlyphArray6x8 = GlyphArray<6, 8, N>;

template <size_t N>
using GlyphArray8x8 = GlyphArray<8, 8, N>;

class HeaderGlyphArray6x8 : public GlyphArray6x8<3> {
  public:
    using Base = GlyphArray6x8<3>;
    constexpr HeaderGlyphArray6x8(const Glyph &h1, const Glyph &h2,
                                  const Glyph &h3)
        : Base() {
        this->glyphs[0] = h1;
        this->glyphs[1] = h2;
        this->glyphs[2] = h3;
        this->size = 3;
    }

    static constexpr int CharWidth = Base::CharWidth;
    static constexpr int CharHeight = Base::CharHeight;
};

/**
 * @brief Calculate the size of a formatted glyph array for a given number of
 * digits and group size.
 *
 * Examples:
 * Hexadecimal: DEADBEAF -> DEAD_BEAF (group size 4, 1 separator)
 * Decimal: -1234567890 -> -1_234_567_890 (group size 3, 2 separators + 1 sign)
 *
 * @param digitCount The number of digits in the number
 * @param groupSize The size of each group of digits (e.g. 4 for binary, 3 for
 * octal, etc.)
 * @return constexpr size_t The size of the formatted glyph array
 */
constexpr size_t
GlyphFormatSize(size_t digitCount, int groupSize, bool signable = false) {
    if (groupSize <= 0) {
        return digitCount;
    }
    size_t separatorCount = (digitCount - 1) / groupSize;
    // +1 for potential sign
    return digitCount + separatorCount + (signable ? 1 : 0);
}

/**
 * @brief Glyph array for formatted numbers, with separators and optional sign.
 *
 * @tparam Separator The font character used as a separator (e.g. ',' for
 * decimal, '_' for hexadecimal and octal)
 * @tparam GroupSize The size of each group of digits (e.g. 4 for binary, 3 for
 * octal, etc.)
 * @tparam Signable Whether the number has a sign (true for decimal, false for
 * hexadecimal and octal)
 * @tparam N The maximum number of digits in the number
 */
template <FontType Separator, int GroupSize, bool Signable, size_t N>
class GlyphFormatArray6x8
    : public GlyphArray6x8<GlyphFormatSize(N, GroupSize, Signable)> {
  private:
    using Base = GlyphArray6x8<GlyphFormatSize(N, GroupSize, Signable)>;

    template <size_t I>
    constexpr void
    InsertDigit(const GlyphArray6x8<N> &src, size_t digitCount) {
        if constexpr (I < N) {

            if (I < digitCount) {

                auto glyph = src[I];
                this->Insert(glyph);

                constexpr bool enableSep = (GroupSize > 0);

                if constexpr (enableSep) {

                    bool isLast = (I + 1 == digitCount);

                    size_t remaining = digitCount - I - 1;

                    bool needSep = (!isLast) && (remaining % GroupSize == 0);

                    if (needSep) {
                        this->Insert(Glyph(Separator));
                    }
                }
            }
        }
    }

    template <size_t... I>
    constexpr void
    InsertDigitsImpl(const GlyphArray6x8<N> &src, size_t digitCount,
                     std::index_sequence<I...>) {
        (InsertDigit<I>(src, digitCount), ...);
    }

    constexpr void
    InsertDigits(const GlyphArray6x8<N> &src) {
        InsertDigitsImpl(src, src.Size(), std::make_index_sequence<N>{});
    }

  public:
    constexpr GlyphFormatArray6x8(const GlyphArray6x8<N> &glyphArray) : Base() {
        const size_t digitCount = glyphArray.Size();

        // ---------- padding ----------
        if constexpr (!Signable && GroupSize > 0) {

            size_t zerosCount =
                (GroupSize - (digitCount % GroupSize)) % GroupSize;

            for (size_t i = 0; i < zerosCount; i++) {
                this->Insert(Glyph(Base::fontZero));
            }
        }

        // ---------- sign ----------
        if constexpr (Signable) {

            if (glyphArray.Negative()) {
                this->Insert(Glyph(Base::fontMinus));
            }
        }

        // ---------- digits ----------
        InsertDigits(glyphArray);
    }

    static constexpr int CharWidth = Base::CharWidth;
    static constexpr int CharHeight = Base::CharHeight;
};

/**
 * @brief Glyph array for hexadecimal numbers, with group size of 4 and
 * separator '_'.
 *
 */
using HexGlyphArray6x8 =
    GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxHexDigits>;
/**
 * @brief Glyph array for decimal numbers, with a sign and group size of 3 and
 * separator ','.
 *
 */
using DecGlyphArray6x8 =
    GlyphFormatArray6x8<Font6x8Comma, 3, true, Number::MaxDecDigits>;

/**
 * @brief Glyph array for octal numbers, with group size of 3 and separator '_'.
 *
 */
using OctGlyphArray6x8 =
    GlyphFormatArray6x8<FontEmpty, 3, false, Number::MaxOctDigits>;

/**
 * @brief Glyph array for binary numbers, with group size of 4 and separator
 * '_'.
 *
 */
class BinGlyphArray6x8
    : public GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxBinDigits> {
  public:
    constexpr BinGlyphArray6x8(
        const GlyphArray6x8<Number::MaxBinDigits> &glyphArray)
        : GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxBinDigits>(
              GlyphFormatArray6x8<FontEmpty, 16, false, Number::MaxBinDigits>(
                  glyphArray)) {}
};

}; // namespace HexCalc
