/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "layer.h"
#include "traits.h"

namespace HexCalc {

class Display : private NonCopyable {
  public:
};

/**
 * @brief The main view of the calculator, which is displayed on the top screen.
 *
 */
class MainDisplay : public Display {
  public:
    MainDisplay(void);

    void PutGlyph(int16_t x, int16_t y, const Glyph &glyph);

    static constexpr int Bpp = 4;
    static constexpr int TileSize = 8 * 8;
    /**
     * @brief all 4 layers are text layers
     *
     */
    static constexpr auto VideoMode = MODE_0_2D;
    static constexpr auto BgType = BgType_Text4bpp;
    static constexpr auto BgSize = BgSize_T_256x256;
    static constexpr int BGNum = 4;
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
