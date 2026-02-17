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
 * @brief Glyph may have 1 or 2 tiles. If it has 1 tile, up is ignored.
 *
 */
class Glyph {
  public:
    constexpr Glyph(FontType up, FontType down, bool upHFlip = false,
                    bool downHFlip = false)
        : up(up), down(down), upHFlip(upHFlip), upVFlip(false),
          downHFlip(downHFlip), downVFlip(false) {}

    /**
     * @brief Load a glyph from a font.
     *
     * @param font The font to load the glyph from.
     * @return The loaded glyph or InvalidGlyph if the font is invalid.
     */
    static constexpr Glyph From(FontType font);

  private:
    FontType up;
    FontType down;

    bool upHFlip;
    bool upVFlip;
    bool downHFlip;
    bool downVFlip;
};

constexpr Glyph InvalidGlyph = Glyph(FontEmpty, FontEmpty);

constexpr Glyph
Glyph::From(FontType font) {
    switch (font) {
    case Font6x8A:
        return Glyph(0, 1);
    case Font6x8B:
        return Glyph(2, 3);
    case Font6x8C:
        return Glyph(0, 4);
    case Font6x8D:
        return Glyph(22, 23);
    case Font6x8E:
        return Glyph(0, 5);
    case Font6x8F:
        return Glyph(7, 6);
    case Font6x8Zero:
        return Glyph(8, 9);
    case Font6x8One:
        return Glyph(11, 10);
    case Font6x8Two:
        return Glyph(8, 13);
    case Font6x8Three:
        return Glyph(8, 13);
    case Font6x8Four:
        return Glyph(15, 14);
    case Font6x8Five:
        return Glyph(16, 17);
    case Font6x8Six:
        return Glyph(8, 18);
    case Font6x8Seven:
        return Glyph(16, 19);
    case Font6x8Eight:
        return Glyph(8, 20);
    case Font6x8Nine:
        return Glyph(8, 21);
    case Font6x8HH:
        return Glyph(25, 24);
    case Font6x8EH:
        return Glyph(16, 26);
    case Font6x8XH:
        return Glyph(25, 27);
    case Font6x8DH:
        return Glyph(28, 29);
    case Font6x8CH:
        return Glyph(28, 30);
    case Font6x8OH:
        return Glyph(8, 31);
    case Font6x8TH:
        return Glyph(16, 32);
    case Font6x8BH:
        return Glyph(28, 33);
    case Font6x8IH:
        return Glyph(16, 34);
    case Font6x8NH:
        return Glyph(25, 35);
    case Font6x8Equal:
        return Glyph(0, 36);
    case Font6x8Plus:
        return Glyph(0, 37);
    case Font6x8Minus:
        return Glyph(0, 38);
    case Font8x8A:
        return Glyph(0, 39);
    case Font8x8B:
        return Glyph(40, 41);
    case Font8x8C:
        return Glyph(0, 42);
    case Font8x8D:
        return Glyph(40, 41, true, true);
    case Font8x8E:
        return Glyph(0, 43);
    case Font8x8F:
        return Glyph(45, 44);
    case Font8x8One:
        return Glyph(46, 47);
    case Font8x8Two:
        return Glyph(48, 49);
    case Font8x8Three:
        return Glyph(48, 50);
    case Font8x8Comma:
        return Glyph(51, 52);
    case Font8x8Four:
        return Glyph(53, 54);
    case Font8x8Five:
        return Glyph(55, 56);
    case Font8x8Six:
        return Glyph(57, 58);
    case Font8x8Seven:
        return Glyph(59, 60);
    case Font8x8Eight:
        return Glyph(61, 62);
    case Font8x8Nine:
        return Glyph(61, 63);
    case Font8x8Zero:
        return Glyph(61, 64);
    case Font6x8Comma:
        return Glyph(0, 65);
    case FontColoredU:
        return Glyph(66, 67);
    case FontColoredI:
        return Glyph(68, 69);
    case FontColoredN:
        return Glyph(70, 71);
    case FontColoredT:
        return Glyph(68, 72);
    case FontColoredEight:
        return Glyph(75, 74);
    case FontColoredOne:
        return Glyph(76, 77);
    case FontColoredSix:
        return Glyph(78, 80);
    case FontColoredThree:
        return Glyph(81, 82);
    case FontColoredTwo:
        return Glyph(81, 83);
    case FontColoredFour:
        return Glyph(84, 85);
    case Font6x8LBrac:
        return Glyph(89, 90);
    case Font6x8RBrac:
        return Glyph(91, 92);
    case Font6x8LShift:
        return Glyph(0, 103);
    case Font6x8RShift:
        return Glyph(0, 104);
    case Font6x8O:
        return Glyph(0, 101);
    case Font6x8M:
        return Glyph(0, 102);
    case Font6x8And:
        return Glyph(93, 94);
    case Font6x8Or:
        return Glyph(95, 96);
    case Font6x8Multiply:
        return Glyph(0, 97);
    case Font6x8Divide:
        return Glyph(0, 98);
    default:
        return InvalidGlyph;
    }
}

}; // namespace HexCalc
