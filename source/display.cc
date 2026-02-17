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

MainDisplay::MainDisplay(void) {
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
        this->bg[i] = bgInit(0, BgType, BgSize, mapBase, 0);
    }

    // copy font palette
    // TODO more themes
    dmaCopy(mainFontPal, BG_PALETTE, mainFontPalLen);
    setBackdropColor(0xFFFF);

    // cope font tiles
    dmaCopy(mainFontTiles, bgGetGfxPtr(bg[0]), mainFontTilesLen);
}

SubDisplay::SubDisplay(void) {
    // button style & animation will be handled by modifing palette
    videoSetModeSub(VideoMode);
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    // use layer 3
    this->bg = bgInitSub(3, SubType, SubSize, 0, 0);
    // TODO load image
}
