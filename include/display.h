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
     * @tparam CharWidth The width of each character in pixels. It can be 6
     * or 8.
     * @param glyphArray Array of glyphs to print
     * @param length Length of the glyph array
     * @param skip Number of glyphs to skip at the beginning of the line
     * @param start The starting point to print the line
     */
    template <int CharWidth = 6>
    void
    PrintLine(const Glyph *glyphArray, size_t length, int skip,
              Point &start) const {
        auto x = start.x;
        auto y = start.y;
        debugf("PrintLine: start=(%d, %d), skip=%d, length=%d\n", x, y, skip,
               length);
        for (size_t i = 0; i < length; i++) {
            auto x = start.x + (skip + i) * CharWidth;
            this->PrintGlyph(x, y, glyphArray[i]);
        }
    }

    template <typename Iterable>
    void
    PrintLine(const Iterable &glyphRange, int skip, Point &start,
              int CharWidth) const {
        auto x = start.x;
        auto y = start.y;
        size_t index = 0;
        debugf("PrintLine(range): start=(%d, %d), skip=%d, length=%zu\n", x, y,
               skip, index);
        for (const auto &glyph : glyphRange) {
            auto glyphX = start.x + (skip + index) * CharWidth;
            this->PrintGlyph(glyphX, y, glyph);
            index++;
        }
    }

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
    static constexpr auto SubSize = BgSize_T_256x256;

  private:
    Layer<SubDisplay> layer;
};

}; // namespace HexCalc
