/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "font.h"

namespace HexCalc {

template <typename DisplayType>
class Layer {
  public:
    Layer(int16_t offsetX = 0, int16_t offsetY = 0)
        : bg(-1), offsetX(offsetX), offsetY(offsetY) {}

    void
    Init(int layer, int mapBase, int tileBase) {
        constexpr auto bgType = DisplayType::BgType;
        constexpr auto bgSize = DisplayType::BgSize;

        bg = bgInit(layer, bgType, bgSize, mapBase, 0);
        bgScroll(bg, offsetX, offsetY);
    }

    int
    GetBg(void) const {
        return bg;
    }

    void
    Put(uint8_t x, uint8_t y, uint16_t tile, bool hFlip = false,
        bool vFlip = false) {
        uint16_t *mapPtr = bgGetMapPtr(bg);
        debugf("Put tile %d at (%d, %d) on bg %d\n", tile, x, y, bg);
        mapPtr[(y * Width) + x] =
            tile | (hFlip ? BIT(10) : 0) | (vFlip ? BIT(11) : 0);
    }

    void
    PutGlyph(uint8_t x, uint8_t y, const Glyph &glyph) {
        auto _y = glyph.UnderBaseline() ? (y + 1) : (y);
        this->Put(x, _y, glyph.Upper(), glyph.UpperHFlip(), glyph.UpperVFlip());
        this->Put(x, _y + 1, glyph.Lower(), glyph.LowerHFlip(),
                  glyph.LowerVFlip());
    }

    void
    Clear(void) {
        uint16_t *mapPtr = bgGetMapPtr(bg);
        dmaFillHalfWords(0, mapPtr, Width * Height * sizeof(uint16_t));
    }

  private:
    int bg;
    int16_t offsetX;
    int16_t offsetY;

    static constexpr int Width = SCREEN_WIDTH / 8;
    static constexpr int Height = SCREEN_HEIGHT / 8;
};

}; // namespace HexCalc
