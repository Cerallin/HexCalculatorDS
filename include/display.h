/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "input.h"
#include "layer.h"
#include "traits.h"

namespace HexCalc {

class Display : private NonCopyable {};

/**
 * @brief The main view of the calculator, which is displayed on the top screen.
 *
 */
class MainDisplay : public Display {
  public:
    MainDisplay(void);

    /**
     * @brief Print a glyph at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param glyph The glyph to print
     */
    void PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const;

    /**
     * @brief Print a tile at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param tile The tile to print
     */
    void PutTile(int16_t x, int16_t y, FontType tile, bool hFlip = false,
                 bool vFlip = false) const;

    /**
     * @brief Print a line of glyphs.
     *
     * @tparam Iterable An iterable type that contains Glyphs
     * @param glyphRange The range of glyphs to print
     * @param skip The number of glyphs to skip at the beginning of the line
     * @param start The starting position of the line
     * @param charWidth The width of each character in pixels
     */
    template <typename GlyphIterable>
    void
    PrintLine(const GlyphIterable &glyphRange, Point &start) const {
        constexpr auto charWidth = GlyphIterable::CharWidth;
        auto x = start.x;
        auto y = start.y;
        // print glyphs
        size_t index = 0;
        for (const auto &glyph : glyphRange) {
            auto glyphX = start.x + (index * charWidth);
            this->PrintGlyph(glyphX, y, glyph);
            index++;
        }
    }

    void ClearLine(Point &start, int charWidth);

    static constexpr int Bpp = 4;
    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;
    static constexpr int TileSize = TileWidth * TileHeight;
    /**
     * @brief all 4 layers are text layers
     *
     */
    static constexpr auto VideoMode = MODE_0_2D;
    static constexpr auto BgType = BgType_Text4bpp;
    static constexpr auto BgSize = BgSize_T_256x256;
    static constexpr int BGNum = 4;
    static constexpr int OffsetPerBG = TileWidth / BGNum;
    static constexpr int MaxTileNum = 256;

    /**
     * @brief Global scroll offset X for all layers.
     *
     */
    static constexpr uint16_t offsetX = 0;

    /**
     * @brief Global scroll offset Y for all layers.
     *
     */
    static constexpr uint16_t offsetY = 0;

  private:
    Layer<MainDisplay> layers[BGNum];
};

/**
 * @brief The sub view of the calculator, which is displayed on the bottom
 * screen.
 *
 */
class SubDisplay : public Display {
  public:
    SubDisplay(void);

    /**
     * @brief
     *
     */
    static constexpr auto VideoMode = MODE_3_2D;
    static constexpr auto SubType = BgType_Bmp8;
    static constexpr auto SubSize = BgSize_B8_256x256;

  private:
    Layer<SubDisplay> layer;
};

}; // namespace HexCalc
