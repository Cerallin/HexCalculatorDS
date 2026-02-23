/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "display.h"
#include "font.h"

namespace HexCalc {

class MainDisplay;
class SubDisplay;

template <typename Class, typename DisplayType>
class BasicLayer {
  public:
    BasicLayer(int16_t offsetX = 0, int16_t offsetY = 0)
        : bg(-1), mapPtr(nullptr), offsetX(offsetX), offsetY(offsetY) {}

    void
    Init(int layer, int mapBase, int tileBase) {
        constexpr auto bgType = Class::BgType;
        constexpr auto bgSize = Class::BgSize;

        // save handles
        if constexpr (std::is_same_v<DisplayType, MainDisplay>) {
            bg = bgInit(layer, bgType, bgSize, mapBase, tileBase);
        } else if constexpr (std::is_same_v<DisplayType, SubDisplay>) {
            bg = bgInitSub(layer, bgType, bgSize, mapBase, tileBase);
        } else {
            // should never reach here
        }
        mapPtr = bgGetMapPtr(bg);

        ResetScroll();
    }

    int
    GetBg(void) const {
        return bg;
    }

    void
    ResetScroll(void) const {
        bgSetScroll(bg, offsetX, offsetY);
    }

  protected:
    uint16_t *mapPtr;
    int bg;

  private:
    int16_t offsetX;
    int16_t offsetY;
};

template <typename DisplayType>
class TileLayer : public BasicLayer<TileLayer<DisplayType>, DisplayType> {
  public:
    TileLayer(int16_t offsetX = 0, int16_t offsetY = 0)
        : BasicLayer<TileLayer<DisplayType>, DisplayType>(offsetX, offsetY) {}

    void
    Put(uint8_t x, uint8_t y, uint16_t tile, bool hFlip = false,
        bool vFlip = false) const {
        this->mapPtr[(y * Width) + x] =
            tile | (hFlip ? BIT(10) : 0) | (vFlip ? BIT(11) : 0);
    }

    void
    PutGlyph(uint8_t x, uint8_t y, const Glyph &glyph) const {
        auto _y = glyph.UnderBaseline() ? (y + 1) : (y);
        this->Put(x, _y, glyph.Upper(), glyph.UpperHFlip(), glyph.UpperVFlip());
        this->Put(x, _y + 1, glyph.Lower(), glyph.LowerHFlip(),
                  glyph.LowerVFlip());
    }

    void
    Clear(void) {
        dmaFillHalfWords(0, this->mapPtr, Width * Height * sizeof(uint16_t));
    }

    static constexpr int TileWidth = DisplayType::TileWidth;
    static constexpr int TileHeight = DisplayType::TileHeight;
    static constexpr size_t Width = SCREEN_WIDTH / TileWidth;
    static constexpr size_t Height = SCREEN_HEIGHT / TileHeight;

    static constexpr auto BgType = BgType_Text4bpp;
    static constexpr auto BgSize = BgSize_T_256x256;

  private:
};

template <typename DisplayType>
class BmpLayer : public BasicLayer<BmpLayer<DisplayType>, DisplayType> {
  public:
    BmpLayer(int16_t offsetX = 0, int16_t offsetY = 0)
        : BasicLayer<BmpLayer<DisplayType>, DisplayType>(offsetX, offsetY) {}

    void
    PutPixel(uint16_t x, uint16_t y, uint16_t color) const {
        this->mapPtr[(y * Width) + x] = color;
    }

    static constexpr size_t Width = 256;
    static constexpr size_t Height = 256;

    static constexpr auto BgType = BgType_Bmp8;
    static constexpr auto BgSize = BgSize_B8_256x256;

    static constexpr int BitmapSize = Width * Height * sizeof(uint8_t);

  private:
    int16_t offsetX;
    int16_t offsetY;
};

}; // namespace HexCalc
