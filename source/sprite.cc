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

#include "subDigitFocus.h"
#include "subNumber.h"
#include "subVersion.h"

#define _(str) versionStrFont(str)

using namespace HexCalc;

MainSpriteManager::MainSpriteManager(void) : SpriteManager<MainDisplay>() {
    vramSetBankE(VRAM_E_MAIN_SPRITE);
    oamInit(&oamState, SpriteMapping_1D_256, false);
    static_assert((sizeof(mainPal) / sizeof(uint16_t)) <= BIT(MainDisplay::Bpp),
                  "Palette size exceeds color format limit");
    dmaCopy(mainPal, SPRITE_PALETTE, sizeof(mainPal));
    oamEnable(&oamState);
}

void
MainSpriteManager::LoadBarTiles(const uint16_t *bgGfx) {
    assert(bgGfx != nullptr);
    auto *gfx = reinterpret_cast<uint8_t *>(SpriteGfx());
    auto *dstBase = &gfx[IndicatorBarTileOffset * TileBytes];
    const auto *srcBase = reinterpret_cast<const uint8_t *>(bgGfx);
    for (size_t i = 0; i < BarTileCount; i++) {
        const size_t srcIndex = static_cast<size_t>(BarTiles[i]);
        dmaCopy(&srcBase[srcIndex * TileBytes], &dstBase[i * TileBytes],
                TileBytes);
    }
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
    // Dedicated bank for DigitFocus sign-color animation (tiles use index 2).
    CopyPaletteBank(DigitFocusPaletteBank, subSpritePal, sizeof(subSpritePal));

    constexpr auto _tileOffset = [](int count) {
        return (count * TileBytes) / sizeof(uint16_t);
    };

    // Load shared graphics for version numbers
    static_assert(
        VersionTileOffset > sizeof(subNumberMapLen) / sizeof(subNumberMap[0]),
        "VersionTileOffset must be greater than subNumberTiles count");
    constexpr int versionOffset = _tileOffset(VersionTileOffset);
    decompress(subVersionTiles, SpriteGfx() + versionOffset, LZ77Vram);
    // Use the same palette for version

    // Load digit focus tile offset
    static_assert(
        DigitFocusTileOffset >
            (sizeof(subNumberMapLen) / sizeof(subNumberMap[0])) +
                (sizeof(subVersionMapLen) / sizeof(subVersionMap[0])),
        "VersionTileOffset must be greater than subNumberTiles count");
    constexpr int digitFocusOffset = _tileOffset(DigitFocusTileOffset);
    dmaCopy(subDigitFocusTiles, SpriteGfx() + digitFocusOffset,
            sizeof(subDigitFocusTiles));

    oamEnable(&oamState);
}

static constexpr int16_t
kerning(FontChar font) {
    if (font == FontVersionDot) {
        return 3;
    } else {
        return 5;
    }
};

void
SubSpriteManager::RegisterVerStr(void) {
    // Initialize the version string
    constexpr auto versionArr = _("v" HEXCALCDS_PROJECT_VERSION);

    int16_t offsetX = 0;
    constexpr int16_t offsetY = 1;
    for (const auto &font : versionArr) {
        auto sp = Add(Point(10 + offsetX, offsetY));
        int tileIndex = font - FontVersion0 + VersionTileOffset + 1;
        // 16 for number tiles, 1 for the backdrop color
        sp->SetTileOffset(tileIndex);
        offsetX += kerning(font);
    }
}
