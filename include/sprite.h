/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "input.h"

#include <nds.h>

namespace HexCalc {

class MainDisplay;
class SubDisplay;

template <typename DisplayType>
class Sprite {
  public:
    static constexpr int TileBytes = 8 * 8 / 2; // 8x8 pixels, 4bpp
    static constexpr auto &oamState =
        std::is_same_v<DisplayType, MainDisplay> ? oamMain : oamSub;
    static constexpr auto SpriteSize = SpriteSize_8x8;
    static constexpr auto SpriteColorFormat = SpriteColorFormat_16Color;

    constexpr Sprite(Point position, const uint16_t *tile, int priority)
        : position(position), tile(tile), priority(priority), dirty(true) {}
    constexpr Sprite(void) : Sprite(Point(0, 0), nullptr, -1) {}

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

    void
    SetPriority(int newPriority) {
        priority = newPriority;
        dirty = true;
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

    SpriteManager(void) : sprites{}, spriteCount(0) {}

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
    Reset(void) {
        // hide all sprites
        for (int i = 0; i < MaxSprites; i++) {
            sprites[i].SetPriority(-1);
        }
        // reset sprite count
        spriteCount = 0;
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

    constexpr static int MaxSprites = 32;

  private:
    using SpriteIndex = ssize_t;

    Sprite<DisplayType> sprites[MaxSprites];
    SpriteIndex spriteCount;

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

class MainDisplay;
class SubDisplay;

class MainSpriteManager : public SpriteManager<MainDisplay> {
  public:
    MainSpriteManager(void);
};

class SubSpriteManager : public SpriteManager<SubDisplay> {
  public:
    SubSpriteManager(void);

    void PrintVersionString(void);
};

}; // namespace HexCalc
