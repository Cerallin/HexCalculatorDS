/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "number.h"
#include "operator.h"

namespace HexCalc {

using FontType = uint8_t;

/**
 * @brief FontChar used for formula and transcode views.
 *
 */
enum FontChar : FontType {
    FontEmpty = 0,
    FontEOS = 0xFF,
    Font6x8Zero = 1,
    Font6x8One,
    Font6x8Two,
    Font6x8Three,
    Font6x8Four,
    Font6x8Five,
    Font6x8Six,
    Font6x8Seven,
    Font6x8Eight,
    Font6x8Nine,
    Font6x8A,
    Font6x8B,
    Font6x8C,
    Font6x8D,
    Font6x8E,
    Font6x8F,
    Font6x8Equal,
    Font6x8LBrac,
    Font6x8RBrac,
    Font6x8LShift,
    Font6x8RShift,
    Font6x8M,
    Font6x8O,
    Font6x8And,
    Font6x8Or,
    Font6x8Multiply,
    Font6x8Divide,
    Font6x8Plus,
    Font6x8Minus,
    Font6x8Comma,
    /**
     * @brief FontChar used for transcode view headers.
     *
     */
    Font6x8HH = 33,
    Font6x8EH,
    Font6x8XH,
    Font6x8DH,
    Font6x8CH,
    Font6x8OH,
    Font6x8TH,
    Font6x8BH,
    Font6x8IH,
    Font6x8NH,
    Font6x8Barrier,

    /**
     * @brief FontChar used for value view.
     *
     */
    Font8x8Zero = 64,
    Font8x8One,
    Font8x8Two,
    Font8x8Three,
    Font8x8Four,
    Font8x8Five,
    Font8x8Six,
    Font8x8Seven,
    Font8x8Eight,
    Font8x8Nine,
    Font8x8A,
    Font8x8B,
    Font8x8C,
    Font8x8D,
    Font8x8E,
    Font8x8F,
    Font8x8Comma,
    Font8x8Barrier,

    /**
     * @brief FontChar used for number type (e.g. UINT32, INT64, etc.).
     *
     */
    FontColoredU = 117,
    FontColoredI,
    FontColoredN,
    FontColoredT,
    FontColoredOne,
    FontColoredTwo,
    FontColoredThree,
    FontColoredFour,
    FontColoredSix,
    FontColoredEight,
    FontColoredBarrier,
};

/**
 * @brief The sign of selected transcode number base.
 *
 */
constexpr int BarTileCount = 3;
constexpr int BarTiles[BarTileCount] = {86, 87, 88};

/**
 * @brief Glyph may have 1 or 2 tiles.
 *
 */
class Glyph {
  public:
    constexpr Glyph(FontType upper, FontType lower, bool upHFlip = false,
                    bool downHFlip = false, bool underBaseline = false)
        : upper(upper), lower(lower), upHFlip(upHFlip), upVFlip(false),
          downHFlip(downHFlip), downVFlip(false), underBaseline(underBaseline) {
    }
    constexpr Glyph(void)
        : upper(FontEmpty), lower(FontEmpty), upHFlip(false), upVFlip(false),
          downHFlip(false), downVFlip(false), underBaseline(false) {}

    /**
     * @brief Load a glyph from a font.
     *
     * @param font The font to load the glyph from.
     * @return The loaded glyph or InvalidGlyph if the font is invalid.
     */
    constexpr Glyph(FontType font);

    FontType
    Upper(void) const {
        return this->upper;
    }

    FontType
    Lower(void) const {
        return this->lower;
    }

    bool
    UpperHFlip(void) const {
        return this->upHFlip;
    }

    bool
    UpperVFlip(void) const {
        return this->upVFlip;
    }

    bool
    LowerHFlip(void) const {
        return this->downHFlip;
    }

    bool
    LowerVFlip(void) const {
        return this->downVFlip;
    }

    bool
    UnderBaseline(void) const {
        return this->underBaseline;
    }

  private:
    FontType upper;
    FontType lower;

    bool upHFlip;
    bool upVFlip;
    bool downHFlip;
    bool downVFlip;
    bool underBaseline;
};

constexpr Glyph InvalidGlyph = Glyph();

constexpr Glyph::Glyph(FontType font)
    : upper(FontEmpty), lower(FontEmpty), upHFlip(false), upVFlip(false),
      downHFlip(false), downVFlip(false), underBaseline(false) {
    switch (font) {
    case Font6x8A:
        *this = Glyph(0, 1);
        break;
    case Font6x8B:
        *this = Glyph(2, 3);
        break;
    case Font6x8C:
        *this = Glyph(0, 4);
        break;
    case Font6x8D:
        *this = Glyph(22, 23);
        break;
    case Font6x8E:
        *this = Glyph(0, 5);
        break;
    case Font6x8F:
        *this = Glyph(7, 6);
        break;
    case Font6x8Zero:
        *this = Glyph(8, 9);
        break;
    case Font6x8One:
        *this = Glyph(11, 10);
        break;
    case Font6x8Two:
        *this = Glyph(8, 12);
        break;
    case Font6x8Three:
        *this = Glyph(8, 13);
        break;
    case Font6x8Four:
        *this = Glyph(15, 14);
        break;
    case Font6x8Five:
        *this = Glyph(16, 17);
        break;
    case Font6x8Six:
        *this = Glyph(8, 18);
        break;
    case Font6x8Seven:
        *this = Glyph(16, 19);
        break;
    case Font6x8Eight:
        *this = Glyph(8, 20);
        break;
    case Font6x8Nine:
        *this = Glyph(8, 21);
        break;
    case Font6x8HH:
        *this = Glyph(25, 24);
        break;
    case Font6x8EH:
        *this = Glyph(16, 26);
        break;
    case Font6x8XH:
        *this = Glyph(25, 27);
        break;
    case Font6x8DH:
        *this = Glyph(28, 29);
        break;
    case Font6x8CH:
        *this = Glyph(8, 30);
        break;
    case Font6x8OH:
        *this = Glyph(8, 31);
        break;
    case Font6x8TH:
        *this = Glyph(16, 32);
        break;
    case Font6x8BH:
        *this = Glyph(28, 33);
        break;
    case Font6x8IH:
        *this = Glyph(16, 34);
        break;
    case Font6x8NH:
        *this = Glyph(25, 35);
        break;
    case Font6x8Equal:
        *this = Glyph(0, 36);
        break;
    case Font6x8Plus:
        *this = Glyph(0, 37);
        break;
    case Font6x8Minus:
        *this = Glyph(0, 38);
        break;
    case Font8x8A:
        *this = Glyph(0, 39);
        break;
    case Font8x8B:
        *this = Glyph(40, 41);
        break;
    case Font8x8C:
        *this = Glyph(0, 42);
        break;
    case Font8x8D:
        *this = Glyph(40, 41, true, true);
        break;
    case Font8x8E:
        *this = Glyph(0, 43);
        break;
    case Font8x8F:
        *this = Glyph(45, 44);
        break;
    case Font8x8One:
        *this = Glyph(46, 47);
        break;
    case Font8x8Two:
        *this = Glyph(48, 49);
        break;
    case Font8x8Three:
        *this = Glyph(48, 50);
        break;
    case Font8x8Comma:
        *this = Glyph(51, 52, false, false, true);
        break;
    case Font8x8Four:
        *this = Glyph(53, 54);
        break;
    case Font8x8Five:
        *this = Glyph(55, 56);
        break;
    case Font8x8Six:
        *this = Glyph(48, 57, true);
        break;
    case Font8x8Seven:
        *this = Glyph(55, 58, true);
        break;
    case Font8x8Eight:
        *this = Glyph(59, 60);
        break;
    case Font8x8Nine:
        *this = Glyph(59, 61);
        break;
    case Font8x8Zero:
        *this = Glyph(59, 62);
        break;
    case Font6x8Comma:
        *this = Glyph(0, 63);
        break;
    case FontColoredU:
        *this = Glyph(64, 65);
        break;
    case FontColoredI:
        *this = Glyph(66, 67);
        break;
    case FontColoredN:
        *this = Glyph(68, 69);
        break;
    case FontColoredT:
        *this = Glyph(66, 70);
        break;
    case FontColoredEight:
        *this = Glyph(73, 72);
        break;
    case FontColoredOne:
        *this = Glyph(74, 75);
        break;
    case FontColoredSix:
        *this = Glyph(76, 78);
        break;
    case FontColoredThree:
        *this = Glyph(76, 79, true);
        break;
    case FontColoredTwo:
        *this = Glyph(76, 80, true);
        break;
    case FontColoredFour:
        *this = Glyph(81, 82);
        break;
    case Font6x8LBrac:
        *this = Glyph(86, 87);
        break;
    case Font6x8RBrac:
        *this = Glyph(88, 89);
        break;
    case Font6x8LShift:
        *this = Glyph(0, 100);
        break;
    case Font6x8RShift:
        *this = Glyph(0, 101);
        break;
    case Font6x8O:
        *this = Glyph(0, 98);
        break;
    case Font6x8M:
        *this = Glyph(0, 99);
        break;
    case Font6x8And:
        *this = Glyph(90, 91);
        break;
    case Font6x8Or:
        *this = Glyph(92, 93);
        break;
    case Font6x8Multiply:
        *this = Glyph(0, 94);
        break;
    case Font6x8Divide:
        *this = Glyph(0, 95);
        break;
    default:
        *this = InvalidGlyph;
        break;
    }
}

}; // namespace HexCalc
