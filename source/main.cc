/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "common.h"
#include "mainFont.h"

#include <stdio.h>

int
main(void) {
    // debug
    consoleDebugInit(DebugDevice_NOCASH);
    fprintf(stderr, "Hello world\n");

    // preparation
    vramSetBankC(VRAM_C_SUB_BG_0x06200000);
    vramSetBankF(VRAM_F_MAIN_BG_0x06000000);

    videoSetMode(MODE_0_2D);

    int bg0 = bgInit(0, BgType_Text4bpp, BgSize_T_256x256, 4, 0);
    bgInit(1, BgType_Text4bpp, BgSize_T_256x256, 5, 0);
    bgInit(2, BgType_Text4bpp, BgSize_T_256x256, 6, 0);
    bgInit(3, BgType_Text4bpp, BgSize_T_256x256, 7, 0);

    // 拷贝调色板
    dmaCopy(mainFontPal, BG_PALETTE, mainFontPalLen);
    // 拷贝 tile 数据
    dmaCopy(mainFontTiles, bgGetGfxPtr(bg0), mainFontTilesLen);
    // 拷贝 map 数据
    dmaCopy(mainFontMap, bgGetMapPtr(bg0), mainFontMapLen);

    // main loop
    while (true) {
        swiWaitForVBlank();
        scanKeys();
    }

    return 0;
}
