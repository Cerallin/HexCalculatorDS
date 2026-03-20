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
    template <size_t M>
    constexpr GlyphArray(const GlyphArray<W, H, M> &other, size_t offset,
                         size_t limit)
        : glyphs{}, size(0), negative(false) {
        auto endIndex = std::min(offset + limit, other.Size());
        for (size_t i = offset; i < endIndex; i++) {
            this->Insert(other[i]);
        }
    }
    explicit constexpr GlyphArray(DigitArray<N> digits, bool reverse = false)
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

    constexpr bool
    Insert(const Glyph &glyph) {
        if (size >= N) {
            return false;
        }

        glyphs[size++] = glyph;

        return true;
    }

    constexpr size_t
    Size(void) const {
        return size;
    }

    using iterator = Glyph *;
    using const_iterator = const Glyph *;

    constexpr const Glyph &
    operator[](size_t index) const {
        return glyphs[index];
    }

    constexpr iterator
    begin(void) {
        return glyphs;
    }
    constexpr iterator
    end(void) {
        return glyphs + size;
    }

    constexpr const_iterator
    begin(void) const {
        return glyphs;
    }
    constexpr const_iterator
    end(void) const {
        return glyphs + size;
    }

    constexpr const_iterator
    cbegin(void) const {
        return glyphs;
    }
    constexpr const_iterator
    cend(void) const {
        return glyphs + size;
    }

    constexpr bool
    Negative(void) const {
        return negative;
    }

    constexpr void
    Clear(void) {
        size = 0;
        negative = false;
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
    // if (digitCount == 0 || groupSize <= 0) {
    //     return digitCount;
    // }
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
 * @tparam PaddingGroupSize The zero-padding group size. Set to 0 to disable
 * padding.
 * @tparam Signable Whether the number has a sign (true for decimal, false for
 * hexadecimal and octal)
 * @tparam N The maximum number of digits in the number
 */
template <FontType Separator, int GroupSize, int PaddingGroupSize,
          bool Signable, int W, int H, size_t N>
class GlyphFormatArray
    : public GlyphArray<W, H, GlyphFormatSize(N, GroupSize, Signable)> {
  private:
    using Base = GlyphArray<W, H, GlyphFormatSize(N, GroupSize, Signable)>;

    constexpr void
    InsertDigit(const Glyph &glyph, size_t index, size_t digitCount) {
        this->Insert(glyph);

        if constexpr (GroupSize > 0) {
            bool isLast = (index + 1 == digitCount);
            size_t remaining = digitCount - index - 1;
            bool needSep = (!isLast) && (remaining % GroupSize == 0);

            if (needSep) {
                this->Insert(Glyph(Separator));
            }
        }
    }

  public:
    explicit constexpr GlyphFormatArray(const GlyphArray<W, H, N> &glyphArray)
        : Base() {
        const size_t digitCount = glyphArray.Size();
        size_t paddingCount = 0;

        // ---------- padding ----------
        if constexpr (PaddingGroupSize > 0) {
            paddingCount =
                (PaddingGroupSize - (digitCount % PaddingGroupSize)) %
                PaddingGroupSize;
        }

        const size_t totalDigitCount = digitCount + paddingCount;

        // ---------- sign ----------
        if constexpr (Signable) {

            if (glyphArray.Negative()) {
                this->Insert(Glyph(Base::fontMinus));
            }
        }

        // ---------- leading zeros ----------
        for (size_t i = 0; i < paddingCount; i++) {
            InsertDigit(Glyph(Base::fontZero), i, totalDigitCount);
        }

        // ---------- digits ----------
        for (size_t i = 0; i < digitCount; i++) {
            InsertDigit(glyphArray[i], paddingCount + i, totalDigitCount);
        }
    }
};

template <FontType Separator, int GroupSize, int PaddingGroupSize,
          bool Signable, size_t N>
using GlyphFormatArray6x8 =
    GlyphFormatArray<Separator, GroupSize, PaddingGroupSize, Signable, 6, 8, N>;

template <FontType Separator, int GroupSize, int PaddingGroupSize,
          bool Signable, size_t N>
using GlyphFormatArray8x8 =
    GlyphFormatArray<Separator, GroupSize, PaddingGroupSize, Signable, 8, 8, N>;

template <NumberBase Base, int W, int H>
struct GlyphFormatTraits;

template <int W, int H>
struct GlyphFormatTraits<Hexadecimal, W, H> {
    static constexpr FontType Separator = FontEmpty;
    static constexpr int GroupSize = 4;
    static constexpr int PaddingGroupSize = 4;
    static constexpr bool Signable = false;
};

template <int W, int H>
struct GlyphFormatTraits<Decimal, W, H> {
    static constexpr FontType Separator =
        (W == 8 && H == 8) ? Font8x8Comma : Font6x8Comma;
    static constexpr int GroupSize = 3;
    static constexpr int PaddingGroupSize = 0;
    static constexpr bool Signable = true;
};

template <int W, int H>
struct GlyphFormatTraits<Octal, W, H> {
    static constexpr FontType Separator = FontEmpty;
    static constexpr int GroupSize = 3;
    static constexpr int PaddingGroupSize = 3;
    static constexpr bool Signable = false;
};

template <int W, int H>
struct GlyphFormatTraits<Binary, W, H> {
    static constexpr FontType Separator = FontEmpty;
    static constexpr int GroupSize = 4;
    static constexpr int PaddingGroupSize = 16;
    static constexpr bool Signable = false;
};

template <NumberBase Base, int W, int H, size_t N>
using NumberGlyphArray =
    GlyphFormatArray<GlyphFormatTraits<Base, W, H>::Separator,
                     GlyphFormatTraits<Base, W, H>::GroupSize,
                     GlyphFormatTraits<Base, W, H>::PaddingGroupSize,
                     GlyphFormatTraits<Base, W, H>::Signable, W, H, N>;

template <NumberBase Base, size_t N>
using NumberGlyphArray6x8 = NumberGlyphArray<Base, 6, 8, N>;

template <NumberBase Base, size_t N>
using NumberGlyphArray8x8 = NumberGlyphArray<Base, 8, 8, N>;

template <NumberBase Base, int W, int H, size_t N>
constexpr auto
MakeFormattedGlyphArray(DigitArray<N> digits, bool reverse = false) {
    GlyphArray<W, H, N> glyphArray(digits, reverse);
    return NumberGlyphArray<Base, W, H, N>(glyphArray);
}

template <int W, int H, size_t N, typename Visitor>
decltype(auto)
VisitFormattedGlyphArray(NumberBase base, DigitArray<N> digits, bool reverse,
                         Visitor &&visitor) {
    switch (base) {
    case Hexadecimal:
        return visitor(
            MakeFormattedGlyphArray<Hexadecimal, W, H>(digits, reverse));
    case Decimal:
        return visitor(MakeFormattedGlyphArray<Decimal, W, H>(digits, reverse));
    case Octal:
        return visitor(MakeFormattedGlyphArray<Octal, W, H>(digits, reverse));
    case Binary:
        return visitor(MakeFormattedGlyphArray<Binary, W, H>(digits, reverse));
    default:
        break;
    }

    assert(false && "Invalid NumberBase");
    return visitor(MakeFormattedGlyphArray<Hexadecimal, W, H>(digits, reverse));
}

/**
 * @brief Glyph array for hexadecimal numbers, with group size of 4 and
 * separator '_'.
 *
 */
using HexGlyphArray6x8 = NumberGlyphArray6x8<Hexadecimal, Number::MaxHexDigits>;
/**
 * @brief Glyph array for decimal numbers, with a sign and group size of 3 and
 * separator ','.
 *
 */
using DecGlyphArray6x8 = NumberGlyphArray6x8<Decimal, Number::MaxDecDigits>;

/**
 * @brief Glyph array for octal numbers, with group size of 3 and separator '_'.
 *
 */
using OctGlyphArray6x8 = NumberGlyphArray6x8<Octal, Number::MaxOctDigits>;

/**
 * @brief Glyph array for binary numbers, with group size of 4 and separator
 * '_'.
 *
 * Example: 0011010110 -> 0000000011010110 -> 0000_0000_1101_0110
 */
using BinGlyphArray6x8 = NumberGlyphArray6x8<Binary, Number::MaxBinDigits>;

template <size_t N>
using HexGlyphArray8x8 = NumberGlyphArray8x8<Hexadecimal, N>;

template <size_t N>
using DecGlyphArray8x8 = NumberGlyphArray8x8<Decimal, N>;

template <size_t N>
using OctGlyphArray8x8 = NumberGlyphArray8x8<Octal, N>;

template <size_t N>
using BinGlyphArray8x8 = NumberGlyphArray8x8<Binary, N>;

constexpr size_t
max(size_t a, size_t b, size_t c, size_t d) {
    return std::max(std::max(a, b), std::max(c, d));
}

template <NumberBase Base>
constexpr size_t
GlyphFormatSize(size_t digitCount) {
    if constexpr (Base == Binary) {
        return GlyphFormatSize(digitCount, 4, false);
    } else if constexpr (Base == Octal) {
        return GlyphFormatSize(digitCount, 3, false);
    } else if constexpr (Base == Decimal) {
        return GlyphFormatSize(digitCount, 3, true);
    } else if constexpr (Base == Hexadecimal) {
        return GlyphFormatSize(digitCount, 4, false);
    } else {
        static_assert(Base == Binary || Base == Octal || Base == Decimal ||
                          Base == Hexadecimal,
                      "Invalid NumberBase");
        return 0;
    }
}

constexpr size_t MaxDisplayDigits =
    max(GlyphFormatSize<Binary>(Number::MaxBinDigits),
        GlyphFormatSize<Octal>(Number::MaxOctDigits),
        GlyphFormatSize<Decimal>(Number::MaxDecDigits),
        GlyphFormatSize<Hexadecimal>(Number::MaxHexDigits));

}; // namespace HexCalc
