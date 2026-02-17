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
    Font6x8HH = 56,
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
    Font8x8Zero = 80,
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
 * @brief Glyph may have 1 or 2 tiles. If it has 1 tile, upper is ignored.
 *
 */
class Glyph {
  public:
    constexpr Glyph(FontType upper, FontType lower, bool upHFlip = false,
                    bool downHFlip = false)
        : upper(upper), lower(lower), upHFlip(upHFlip), upVFlip(false),
          downHFlip(downHFlip), downVFlip(false) {}

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

  private:
    FontType upper;
    FontType lower;

    bool upHFlip;
    bool upVFlip;
    bool downHFlip;
    bool downVFlip;
};

constexpr Glyph InvalidGlyph = Glyph(FontEmpty, FontEmpty);

constexpr Glyph::Glyph(FontType font)
    : upper(FontEmpty), lower(FontEmpty), upHFlip(false), upVFlip(false),
      downHFlip(false), downVFlip(false) {
    switch (font) {
    case Font6x8A:
        Glyph(0, 1);
    case Font6x8B:
        Glyph(2, 3);
    case Font6x8C:
        Glyph(0, 4);
    case Font6x8D:
        Glyph(22, 23);
    case Font6x8E:
        Glyph(0, 5);
    case Font6x8F:
        Glyph(7, 6);
    case Font6x8Zero:
        Glyph(8, 9);
    case Font6x8One:
        Glyph(11, 10);
    case Font6x8Two:
        Glyph(8, 13);
    case Font6x8Three:
        Glyph(8, 13);
    case Font6x8Four:
        Glyph(15, 14);
    case Font6x8Five:
        Glyph(16, 17);
    case Font6x8Six:
        Glyph(8, 18);
    case Font6x8Seven:
        Glyph(16, 19);
    case Font6x8Eight:
        Glyph(8, 20);
    case Font6x8Nine:
        Glyph(8, 21);
    case Font6x8HH:
        Glyph(25, 24);
    case Font6x8EH:
        Glyph(16, 26);
    case Font6x8XH:
        Glyph(25, 27);
    case Font6x8DH:
        Glyph(28, 29);
    case Font6x8CH:
        Glyph(28, 30);
    case Font6x8OH:
        Glyph(8, 31);
    case Font6x8TH:
        Glyph(16, 32);
    case Font6x8BH:
        Glyph(28, 33);
    case Font6x8IH:
        Glyph(16, 34);
    case Font6x8NH:
        Glyph(25, 35);
    case Font6x8Equal:
        Glyph(0, 36);
    case Font6x8Plus:
        Glyph(0, 37);
    case Font6x8Minus:
        Glyph(0, 38);
    case Font8x8A:
        Glyph(0, 39);
    case Font8x8B:
        Glyph(40, 41);
    case Font8x8C:
        Glyph(0, 42);
    case Font8x8D:
        Glyph(40, 41, true, true);
    case Font8x8E:
        Glyph(0, 43);
    case Font8x8F:
        Glyph(45, 44);
    case Font8x8One:
        Glyph(46, 47);
    case Font8x8Two:
        Glyph(48, 49);
    case Font8x8Three:
        Glyph(48, 50);
    case Font8x8Comma:
        Glyph(51, 52);
    case Font8x8Four:
        Glyph(53, 54);
    case Font8x8Five:
        Glyph(55, 56);
    case Font8x8Six:
        Glyph(57, 58);
    case Font8x8Seven:
        Glyph(59, 60);
    case Font8x8Eight:
        Glyph(61, 62);
    case Font8x8Nine:
        Glyph(61, 63);
    case Font8x8Zero:
        Glyph(61, 64);
    case Font6x8Comma:
        Glyph(0, 65);
    case FontColoredU:
        Glyph(66, 67);
    case FontColoredI:
        Glyph(68, 69);
    case FontColoredN:
        Glyph(70, 71);
    case FontColoredT:
        Glyph(68, 72);
    case FontColoredEight:
        Glyph(75, 74);
    case FontColoredOne:
        Glyph(76, 77);
    case FontColoredSix:
        Glyph(78, 80);
    case FontColoredThree:
        Glyph(81, 82);
    case FontColoredTwo:
        Glyph(81, 83);
    case FontColoredFour:
        Glyph(84, 85);
    case Font6x8LBrac:
        Glyph(89, 90);
    case Font6x8RBrac:
        Glyph(91, 92);
    case Font6x8LShift:
        Glyph(0, 103);
    case Font6x8RShift:
        Glyph(0, 104);
    case Font6x8O:
        Glyph(0, 101);
    case Font6x8M:
        Glyph(0, 102);
    case Font6x8And:
        Glyph(93, 94);
    case Font6x8Or:
        Glyph(95, 96);
    case Font6x8Multiply:
        Glyph(0, 97);
    case Font6x8Divide:
        Glyph(0, 98);
    default:
        InvalidGlyph;
    }
}

}; // namespace HexCalc
