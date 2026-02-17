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

    template <typename ModelType>
    void
    Update(const ModelType &model) {
        // TODO update the view with the model
    }

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
