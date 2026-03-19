/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "display.h"
#include "input.h"

#include "subNumber.h"

#include <nds.h>

namespace HexCalc {

template <typename DisplayType>
class Sprite {
  public:
    static constexpr int TileBytes = 8 * 8;
    static constexpr auto &oamState =
        std::is_same_v<DisplayType, MainDisplay> ? oamMain : oamSub;
    static constexpr auto SpriteSize = SpriteSize_8x8;
    static constexpr auto SpriteColorFormat = SpriteColorFormat_16Color;

    constexpr Sprite(Point position, const uint16_t *tile, int priority)
        : position(position), tile(tile), priority(priority), dirty(true) {}
    constexpr Sprite(void) : Sprite(Point(0, 0), nullptr, -1) {}

    void
    FreeGfx(void) {
        oamFreeGfx(&oamState, tile);
    }

    bool
    Valid(void) const {
        return (priority >= 0);
    }

    void
    SetTile(const uint16_t *newTile) {
        if (Valid()) {
            tile = newTile;
            dirty = true;
        }
    }

    void
    SetTileOffset(int tileIndex) {
        SetTile(oamGetGfxPtr(&oamState, tileIndex));
    }

    bool
    Dirty(void) const {
        return dirty;
    }

    void
    Update(int id) {
        if (Valid()) {
            // debug tile
            debugf("tile: %p\n", tile);
            oamSet(&oamState, id, position.x, position.y, priority, 0,
                   SpriteSize, SpriteColorFormat, tile, 0, false, false, false,
                   false, false);
        } else {
            // hide the sprite if it's invalid
            oamSet(&oamState, id, 0, 0, 0, 0, SpriteSize, SpriteColorFormat,
                   nullptr, 0, false, true, false, false, false);
        }

        dirty = false;
    }

  private:
    Point position;
    const uint16_t *tile;
    int priority;
    bool dirty;
};

template <typename DisplayType>
class SpriteManager : NonCopyable {
  public:
    static constexpr auto &oamState = Sprite<DisplayType>::oamState;
    static constexpr auto TileBytes = Sprite<DisplayType>::TileBytes;
    static constexpr auto *
    SpriteGfx(void) {
        if constexpr (std::is_same_v<DisplayType, MainDisplay>) {
            return SPRITE_GFX;
        } else {
            return SPRITE_GFX_SUB;
        }
    }

    SpriteManager(void) {
        // Initialize OAM and VRAM for sprites
        vramSetBankD(VRAM_D_SUB_SPRITE);
        oamInit(&oamState, SpriteMapping_1D_256, false);
        // Load shared graphics for numbers
        decompress(subNumberTiles, SpriteGfx(), LZ77Vram);
        dmaCopy(subNumberPal, SPRITE_PALETTE_SUB, 16 * sizeof(uint16_t));

        oamEnable(&oamState);
    }

    Sprite<DisplayType> *
    Add(Point position, int priority = 0) {
        if (spriteCount >= MaxSprites) {
            // no more free slots
            return nullptr;
        }

        // try find the first free slot
        auto index = findFreeSlot();
        debugf("Adding sprite at index %zd\n", index);
        if (index < 0) {
            // no free slot found
            return nullptr;
        }

        auto &sprite = sprites[index];

        sprite =
            Sprite<DisplayType>(position, oamGetGfxPtr(&oamState, 0), priority);

        spriteCount++;

        return &sprites[index];
    }

    void
    Update(void) {
        for (int i = 0; i < MaxSprites; i++) {
            auto &sprite = sprites[i];

            if (sprite.Dirty()) {
                sprite.Update(i);
            } else {
                // skip unchanged sprites
            }
        }

        oamUpdate(&oamState);
    }

    constexpr static int MaxSprites = 8;

  private:
    using SpriteIndex = ssize_t;

    Sprite<DisplayType> sprites[MaxSprites] = {};
    SpriteIndex spriteCount = 0;

    SpriteIndex
    findFreeSlot(void) const {
        for (SpriteIndex i = 0; i < MaxSprites; i++) {
            if (!sprites[i].Valid()) {
                return i;
            }
        }

        return -1;
    }
};

using SubSpriteManager = SpriteManager<SubDisplay>;
using MainSpriteManager = SpriteManager<MainDisplay>;

}; // namespace HexCalc
