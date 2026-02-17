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
    Layer() : bg(-1), offsetX(0), offsetY(0) {}
    Layer(int bg, int16_t offsetX = 0, int16_t offsetY = 0)
        : bg(bg), offsetX(offsetX), offsetY(offsetY) {
        bgScroll(bg, offsetX, offsetY);
    }

    int
    GetBg(void) const {
        return this->bg;
    }

    void
    Put(uint8_t x, uint8_t y, uint16_t tile) {
        uint16_t *mapPtr = bgGetMapPtr(bg);
        debugf("Put tile %d at (%d, %d) on bg %d\n", tile, x, y, bg);
        mapPtr[(y * Width) + x] = tile;
    }

    void
    PutGlyph(uint8_t x, uint8_t y, const Glyph &glyph) {
        this->Put(x, y, glyph.Upper());
        this->Put(x, y + 1, glyph.Lower());
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
