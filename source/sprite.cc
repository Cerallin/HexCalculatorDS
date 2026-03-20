/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "sprite.h"
#include "display.h"
#include "font.h"
#include "theme.h"

#include "subNumber.h"
#include "subVersion.h"

#define _(str) versionStrFont(str)

using namespace HexCalc;

MainSpriteManager::MainSpriteManager(void) : SpriteManager<MainDisplay>() {
    // not implemented yet
}

SubSpriteManager::SubSpriteManager(void) : SpriteManager<SubDisplay>() {
    // Initialize OAM and VRAM for sprites
    vramSetBankD(VRAM_D_SUB_SPRITE);
    oamInit(&oamState, SpriteMapping_1D_256, false);
    // Load shared graphics for numbers
    decompress(subNumberTiles, SpriteGfx(), LZ77Vram);
    static_assert((sizeof(subSpritePal) / sizeof(uint16_t)) <=
                      BIT(SubDisplay::Bpp),
                  "Palette size exceeds color format limit");
    dmaCopy(subSpritePal, SPRITE_PALETTE_SUB, sizeof(subSpritePal));

    // Load shared graphics for version numbers
    decompress(subVersionTiles, SpriteGfx() + 16 * TileBytes / sizeof(uint16_t),
               LZ77Vram);
    // Use the same palette for version

    // Initialize the version string
    constexpr auto versionArr = _("v" HEXCALCDS_PROJECT_VERSION);

    constexpr auto kerning = [](FontChar font) {
        if (font == FontVersionDot) {
            return 3;
        } else {
            return 5;
        }
    };
    int16_t offsetX = 0;
    int16_t offsetY = 1;
    for (const auto &font : versionArr) {
        auto sp = Add(Point(10 + offsetX, offsetY));
        const auto tileIndex = font - FontVersion0 + 16 + 1;
        // 16 for number tiles, 1 for the backdrop color
        sp->SetTileOffset(tileIndex);
        offsetX += kerning(font);
    }

    oamEnable(&oamState);
}
