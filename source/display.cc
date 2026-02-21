/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "display.h"
#include "common.h"
#include "mainFont.h"

using namespace HexCalc;

static constexpr auto
align64(int x) {
    return (x + 63) & ~63;
};

MainDisplay::MainDisplay(void)
    : // the offset is because of 6x8 tiles on 8x8 tile map
      layers{
          Layer<MainDisplay>(offsetX - (0 * OffsetPerBG), offsetY + 0),
          Layer<MainDisplay>(offsetX - (1 * OffsetPerBG), offsetY + 0),
          Layer<MainDisplay>(offsetX - (2 * OffsetPerBG), offsetY + 0),
          Layer<MainDisplay>(offsetX - (3 * OffsetPerBG), offsetY + 0),
      } {
    videoSetMode(VideoMode);
    // F bank has 16 Kb, enough for <=256 tiles and 4 maps
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);

    // https://mtheall.com/vram.html#T0=2&NT0=256&MB0=4&T1=2&NT1=256&MB1=5&T2=2&NT2=256&MB2=6&T3=2&NT3=256&MB3=7
    for (int i = 0; i < BGNum; i++) {
        constexpr int mapBaseBankSize = 2048; // 2 Kb
        constexpr int byte = 8;
        constexpr int baseOffset =
            ((align64(MaxTileNum) / byte) * Bpp * TileSize) / mapBaseBankSize;
        int mapBase = i + baseOffset;
        layers[i].Init(i, mapBase, 0);
    }

    // copy font palette
    // TODO more themes
    dmaCopy(mainFontPal, BG_PALETTE, mainFontPalLen);
    constexpr auto bgColor = RGB15(31, 31, 30);
    setBackdropColor(bgColor);

    // cope font tiles
    dmaCopy(mainFontTiles, bgGetGfxPtr(this->layers[0].GetBg()),
            mainFontTilesLen);
}

void
MainDisplay::PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = (x / OffsetPerBG) % BGNum;
    auto &layer = layers[_idx];
    layer.PutGlyph(x / TileWidth, y / TileHeight, glyph);
}

void
MainDisplay::PutTile(int16_t x, int16_t y, FontType tile, bool hFlip,
                     bool vFlip) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = (x / OffsetPerBG) % BGNum;
    auto &layer = layers[_idx];
    layer.Put(x / TileWidth, y / TileHeight, tile, hFlip, vFlip);
}

void
MainDisplay::ClearLine(Point &start, int charWidth) {
    auto x = start.x;
    auto y = start.y;

    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);

    auto total = SCREEN_WIDTH / charWidth;
    auto skip = start.x / charWidth;
    auto clearNum = total - skip;
    for (int i = 0; i < clearNum; i++) {
        PrintGlyph(x + i * charWidth, y, FontEmpty);
    }
}

SubDisplay::SubDisplay(void) {
    // button style & animation will be handled by modifing palette
    videoSetModeSub(VideoMode);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    // use layer 3
    auto bg = bgInitSub(3, SubType, SubSize, 0, 0);
    layer = Layer<SubDisplay>(bg);
    // TODO load image
}
