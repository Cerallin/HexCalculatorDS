/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "display.h"
#include "common.h"

// assets
#include "mainFont.h"
#include "subFont.h"
#include "subscreenImage.h"

using namespace HexCalc;

static constexpr int VRAM_C_SIZE = 128 * 1024;    // 128 Kb
static constexpr int bmpBaseBankSize = 16 * 1024; // 16 Kb
static constexpr int mapBaseBankSize = 2048;      // 2 Kb
static constexpr int byteSize = 8;

static constexpr auto
align64(int x) {
    return (x + 63) & ~63;
};

MainDisplay::MainDisplay(void)
    : Display(),
      // the offset is because of 6x8 tiles on 8x8 tile map
      layers{
          TileLayer<MainDisplay>(offsetX - (0 * OffsetPerBG), offsetY + 0),
          TileLayer<MainDisplay>(offsetX - (1 * OffsetPerBG), offsetY + 0),
          TileLayer<MainDisplay>(offsetX - (2 * OffsetPerBG), offsetY + 0),
          TileLayer<MainDisplay>(offsetX - (3 * OffsetPerBG), offsetY + 0),
      } {
    videoSetMode(VideoMode);
    // F bank has 16 Kb, enough for <=256 tiles and 4 maps
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);

    // https://mtheall.com/vram.html#T0=2&NT0=256&MB0=4&T1=2&NT1=256&MB1=5&T2=2&NT2=256&MB2=6&T3=2&NT3=256&MB3=7
    for (int i = 0; i < TileBGNum; i++) {
        constexpr int baseOffset =
            ((align64(MaxTileNum) / byteSize) * Bpp * TileSize) /
            mapBaseBankSize;
        int mapBase = i + baseOffset;
        layers[i].Init(i, mapBase, 0);
    }

    // copy font palette
    // TODO more themes
    dmaCopy(mainFontPal, BG_PALETTE, mainFontPalLen);
    SetBackdrop(31, 31, 30);

    // cope font tiles
    decompress(mainFontTiles, bgGetGfxPtr(this->layers[0].GetBg()), LZ77Vram);
}

void
MainDisplay::PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = (x / OffsetPerBG) % TileBGNum;
    auto &layer = layers[_idx];
    layer.PutGlyph(x / TileWidth, y / TileHeight, glyph);
}

void
MainDisplay::PutTile(int16_t x, int16_t y, FontType tile, bool hFlip,
                     bool vFlip) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = (x / OffsetPerBG) % TileBGNum;
    auto &layer = layers[_idx];
    layer.Put(x / TileWidth, y / TileHeight, tile, hFlip, vFlip);
}

void
MainDisplay::ClearLine(Point &start, int charWidth, bool underline) const {
    auto x = start.x;
    auto y = start.y;

    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);

    auto total = SCREEN_WIDTH / charWidth;
    auto skip = start.x / charWidth;
    auto clearNum = total - skip;
    auto height = underline ? 3 : 2;
    for (int i = 0; i < height; i++) {
        auto _y = y + (i * TileHeight);
        for (int j = 0; j < clearNum; j++) {
            PutTile(x + (j * charWidth), _y, FontEmpty);
        }
    }
}

SubDisplay::SubDisplay(void)
    : Display(), bmpLayer(0, 0),
      tileLayers{
          TileLayer<SubDisplay>(0, 0),     // unused
          TileLayer<SubDisplay>(-152, 0),  // border layer
          TileLayer<SubDisplay>(-164, -8), // text layer
      },
      widthManager(tileLayers[1], tileLayers[2], {0, 0},
                   {0, 0}), // number width manager
      signManager(tileLayers[1], tileLayers[2], {64, 0},
                  {64, 0}) // number sign manager
{
    // button style & animation will be handled by modifing palette
    videoSetModeSub(VideoMode);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);

    constexpr auto bmpOffset =
        (VRAM_C_SIZE - bmpLayer.BitmapSize) / bmpBaseBankSize;
    bmpLayer.Init(3, bmpOffset, 0);

    for (int i = 0; i < TileBGNum; i++) {
        constexpr int baseOffset =
            ((align64(MaxTileNum) / byteSize) * Bpp * TileSize) /
            mapBaseBankSize;
        int mapBase = i + baseOffset;
        tileLayers[i].Init(i, mapBase, 0);
    }

    // copy image palette
    dmaCopy(subscreenImagePal, BG_PALETTE_SUB, subscreenImagePalLen);
    // copy font palette
    dmaCopy(subFontPal, BG_PALETTE_SUB, BIT(Bpp) * sizeof(uint16_t));
    // Set backdrop color
    SetBackdrop(31, 31, 30);

    // set up palettes for button states
    enabledPalette = &BG_PALETTE_SUB[MaxColorCount - (3 * ColorCount)];
    disabledPalette = &BG_PALETTE_SUB[MaxColorCount - (2 * ColorCount)];
    selectedPalette = &BG_PALETTE_SUB[MaxColorCount - ColorCount];

    // draw borders once since they won't change
    widthManager.DrawBorders(0, 0);
    signManager.DrawBorders(0, 0);

    // decompress image bitmap (LZ77) into VRAM
    decompress(subscreenImageBitmap, bgGetGfxPtr(bmpLayer.GetBg()), LZ77Vram);

    // decompress font tiles (LZ77) into VRAM
    decompress(subFontTiles, bgGetGfxPtr(this->tileLayers[0].GetBg()),
               LZ77Vram);
}

void
SubDisplay::DisableButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[16 + (index * ColorCount)];
    dmaCopy(disabledPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::EnableButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[16 + (index * ColorCount)];
    dmaCopy(enabledPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::SelectButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[16 + (index * ColorCount)];
    dmaCopy(selectedPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::UpdateWidthDrawer(NumberWidth width) {
    widthManager.DrawText(width, 0, 0);
}

void
SubDisplay::UpdateSignDrawer(NumberSign sign) {
    signManager.DrawText(sign, 0, 0);
}

Sprite<SubDisplay> *
SubDisplay::AddSprite(Point position, int priority) {
    return sm.Add(position, priority);
}
