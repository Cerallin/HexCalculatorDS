/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "display.h"
#include "common.h"
#include "theme.h"

// assets
#include "mainFont.h"
#include "subBinaryFont.h"
#include "subFont.h"
#include "subscreenBinaryImage.h"
#include "subscreenImage.h"

using namespace HexCalc;

static constexpr int VRAM_C_SIZE = 128 * 1024;    // 128 Kb
static constexpr int bmpBaseBankSize = 16 * 1024; // 16 Kb
static constexpr int mapBaseBankSize = 2048;      // 2 Kb
static constexpr int byteSize = 8;
// grit exports 256x192@8 bitmaps (visible screen)
static constexpr size_t ScreenBitmapSize = SCREEN_WIDTH * SCREEN_HEIGHT;

namespace {
alignas(4) uint8_t decodedInputBitmap[ScreenBitmapSize];
alignas(4) uint8_t decodedBinaryBitmap[ScreenBitmapSize];
} // namespace

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
    dmaCopy(mainPal, BG_PALETTE, sizeof(mainPal));
    SetBackdrop(COLOR_COMMON_BG);

    // cope font tiles
    decompress(mainFontTiles, bgGetGfxPtr(this->layers[0].GetBg()), LZ77Vram);
}

void
MainDisplay::PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = getLayerIndex<TileBGNum, OffsetPerBG>(x);
    layers[_idx].PutGlyph(
        static_cast<uint8_t>(static_cast<unsigned>(x) / TileWidth),
        static_cast<uint8_t>(static_cast<unsigned>(y) / TileHeight), glyph);
}

void
MainDisplay::PutTile(int16_t x, int16_t y, FontType tile, bool hFlip,
                     bool vFlip) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = getLayerIndex<TileBGNum, OffsetPerBG>(x);
    layers[_idx].Put(
        static_cast<uint8_t>(static_cast<unsigned>(x) / TileWidth),
        static_cast<uint8_t>(static_cast<unsigned>(y) / TileHeight), tile,
        hFlip, vFlip);
}

void
MainDisplay::ClearLine(const Point &start, int charWidth,
                       bool underline) const {
    assert(start.x % OffsetPerBG == 0);
    assert(start.y % TileHeight == 0);
    assert(charWidth > 0);
    assert(charWidth % OffsetPerBG == 0);

    const int total = SCREEN_WIDTH / charWidth;
    const int skip = start.x / charWidth;
    const int clearNum = total - skip;
    const int height = underline ? 3 : 2;
    // Runtime equivalent of PrintLine's layerStep (charWidth is not constexpr).
    const int layerStep = (charWidth / OffsetPerBG) & (TileBGNum - 1);
    const int startLayer = getLayerIndex<TileBGNum, OffsetPerBG>(start.x);
    const uint8_t startTileX =
        static_cast<uint8_t>(static_cast<unsigned>(start.x) / TileWidth);

    for (int i = 0; i < height; i++) {
        const uint8_t tileY = static_cast<uint8_t>(
            static_cast<unsigned>(start.y + (i * TileHeight)) / TileHeight);
        int16_t px = start.x;
        uint8_t tileX = startTileX;
        int layerIdx = startLayer;

        for (int j = 0; j < clearNum; j++) {
            layers[layerIdx].Put(tileX, tileY, FontEmpty);
            px += charWidth;
            tileX = static_cast<uint8_t>(static_cast<unsigned>(px) / TileWidth);
            layerIdx = (layerIdx + layerStep) & (TileBGNum - 1);
        }
    }
}

void
MainDisplay::ClearLineBand(const Point &start, int pixelWidth,
                           bool underline) const {
    assert(start.x % OffsetPerBG == 0);
    assert(start.y % TileHeight == 0);
    assert(pixelWidth > 0);
    assert(pixelWidth % OffsetPerBG == 0);

    const int height = underline ? 3 : 2;
    const int clearNum = pixelWidth / OffsetPerBG;
    const int startLayer = getLayerIndex<TileBGNum, OffsetPerBG>(start.x);
    const uint8_t startTileX =
        static_cast<uint8_t>(static_cast<unsigned>(start.x) / TileWidth);

    for (int i = 0; i < height; i++) {
        const uint8_t tileY = static_cast<uint8_t>(
            static_cast<unsigned>(start.y + (i * TileHeight)) / TileHeight);
        int16_t px = start.x;
        uint8_t tileX = startTileX;
        int layerIdx = startLayer;

        for (int j = 0; j < clearNum; j++) {
            layers[layerIdx].Put(tileX, tileY, FontEmpty);
            px += OffsetPerBG;
            tileX = static_cast<uint8_t>(static_cast<unsigned>(px) / TileWidth);
            layerIdx = (layerIdx + 1) & (TileBGNum - 1);
        }
    }
}

SubDisplay::SubDisplay(void)
    : Display(), bmpLayer(0, 0),
      tileLayers{
          TileLayer<SubDisplay>(offsetX - (0 * OffsetPerBG), offsetY + 0),
          TileLayer<SubDisplay>(offsetX - (1 * OffsetPerBG), offsetY + 0),
      },
      widthManager(tileLayers, {152, 0}, {164, 8}), // number width manager
      signManager(tileLayers, {216, 0}, {228, 8})   // number sign manager
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

    // set up palettes for button states
    enabledPalette = &BG_PALETTE_SUB[MaxColorCount - (3 * ColorCount)];
    disabledPalette = &BG_PALETTE_SUB[MaxColorCount - (2 * ColorCount)];
    selectedPalette = &BG_PALETTE_SUB[MaxColorCount - ColorCount];

    // draw borders once since they won't change
    widthManager.DrawBorders(0, 0);
    signManager.DrawBorders(0, 0);

    // decompress font tiles (LZ77) into VRAM
    decompress(subFontTiles, bgGetGfxPtr(this->tileLayers[0].GetBg()),
               LZ77Vram);

    // decompress binary tiles (LZ77) into VRAM after the font tiles
    auto gfxPtr = bgGetGfxPtr(this->tileLayers[0].GetBg());
    constexpr int gfxOffset = subFontMapLen * byteSize;
    decompress(subBinaryFontTiles, &gfxPtr[gfxOffset], LZ77Vram);

    // Pre-decode subscreen bitmaps into main RAM for fast view switching
    decompress(subscreenImageBitmap, decodedInputBitmap, LZ77);
    decompress(subscreenBinaryImageBitmap, decodedBinaryBitmap, LZ77);

    InitializePalette();
}

void
SubDisplay::PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = getLayerIndex<TileBGNum, OffsetPerBG>(x);
    tileLayers[_idx].PutGlyph(
        static_cast<uint8_t>(static_cast<unsigned>(x) / TileWidth),
        static_cast<uint8_t>(static_cast<unsigned>(y) / TileHeight), glyph);
}

void
SubDisplay::PutTile(int16_t x, int16_t y, FontType tile, bool hFlip,
                    bool vFlip) const {
    assert(x % OffsetPerBG == 0);
    assert(y % TileHeight == 0);
    auto _idx = getLayerIndex<TileBGNum, OffsetPerBG>(x);
    tileLayers[_idx].Put(
        static_cast<uint8_t>(static_cast<unsigned>(x) / TileWidth),
        static_cast<uint8_t>(static_cast<unsigned>(y) / TileHeight), tile,
        hFlip, vFlip);
}

void
SubDisplay::InitializePalette(void) {
    // copy font palette
    static_assert((sizeof(subPal) / sizeof(subPal[0])) <= MAX_4BPP_PAL_COUNT,
                  "sub palette has too many colors");
    dmaCopy(subPal, BG_PALETTE_SUB, sizeof(subPal));
    // Set backdrop color
    SetBackdrop(COLOR_COMMON_BG);
}

void
SubDisplay::SetupView(Image image) {
    const uint16_t *palette = nullptr;
    const uint8_t *bitmap = nullptr;

    switch (image) {
    case Image::InputImage:
        palette = subscreenImagePal;
        bitmap = decodedInputBitmap;
        break;
    case Image::BinaryImage:
        palette = subscreenBinaryImagePal;
        bitmap = decodedBinaryBitmap;
        break;
    }

    // copy image palette
    dmaCopy(&palette[MAX_4BPP_PAL_COUNT], &BG_PALETTE_SUB[MAX_4BPP_PAL_COUNT],
            (MAX_8BPP_PAL_COUNT - MAX_4BPP_PAL_COUNT) * sizeof(uint16_t));

    // copy pre-decoded bitmap into VRAM
    dmaCopy(bitmap, bgGetGfxPtr(bmpLayer.GetBg()), ScreenBitmapSize);
}

void
SubDisplay::DisableButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[MAX_4BPP_PAL_COUNT + (index * ColorCount)];
    dmaCopy(disabledPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::EnableButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[MAX_4BPP_PAL_COUNT + (index * ColorCount)];
    dmaCopy(enabledPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::SelectButton(int index) {
    // 1 for the backdrop color
    uint16_t *dest = &BG_PALETTE_SUB[MAX_4BPP_PAL_COUNT + (index * ColorCount)];
    dmaCopy(selectedPalette, dest, ColorCount * sizeof(uint16_t));
}

void
SubDisplay::SetButtonSignColor(int index, uint16_t color) {
    constexpr size_t signOffset = 4;
    const size_t offset =
        MAX_4BPP_PAL_COUNT + (index * ColorCount) + signOffset;
    uint16_t *dest = &BG_PALETTE_SUB[offset];
    *dest = color;
}

void
SubDisplay::UpdateWidthDrawer(NumberWidth width) {
    widthManager.DrawText(width, 0, 0);
}

void
SubDisplay::UpdateSignDrawer(NumberSign sign) {
    signManager.DrawText(sign, 0, 0);
}

void
SubDisplay::DrawDrawerBorders(void) {
    widthManager.DrawBorders(0, 0);
    signManager.DrawBorders(0, 0);
}

Sprite<SubDisplay> *
SubDisplay::AddSprite(Point position, int priority, bool hFlip, bool vFlip) {
    return sm.Add(position, priority, hFlip, vFlip);
}

void
SubDisplay::ResetSprites(void) {
    sm.Reset();
}

void
SubDisplay::CleanLayers(void) {
    for (int layer = 0; layer < TileBGNum; ++layer) {
        tileLayers[layer].Clear();
    }
}

void
SubDisplay::ShowVerStr(void) {
    sm.RegisterVerStr();
}
