/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "input.h"
#include "layer.h"
#include "sprite.h"
#include "traits.h"

namespace HexCalc {

namespace {

/**
 * @brief Map a pixel X to the BG layer that owns that vertical strip.
 *
 * Main screen uses TileBGNum layers, each scrolled by OffsetPerBG pixels, so
 * consecutive 2px (main) / 4px (sub) columns land on alternating layers.
 * Unsigned divide + mask avoids the signed-div/mod sequence that ARM Thumb
 * emits for `(x / OffsetPerBG) % TileBGNum`.
 */
template <int TileBGNum, int OffsetPerBG>
constexpr int
getLayerIndex(int16_t x) {
    static_assert((TileBGNum & (TileBGNum - 1)) == 0,
                  "TileBGNum must be a power of two");
    return (static_cast<unsigned>(x) / OffsetPerBG) & (TileBGNum - 1);
}

} // namespace

template <class Derived, typename DisplayType, int BorderWidth,
          int BorderHeight, int TextCount, int TextGlyphCount>
class DrawerManager {
  public:
    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;

    constexpr DrawerManager(TileLayer<DisplayType> *layers, Point borderOffset,
                            Point textOffset)
        : layers(layers), borderOffset(borderOffset), textOffset(textOffset) {}

    void
    DrawBorders(int x, int y) {
        assert(x % DisplayType::OffsetPerBG == 0);
        assert(y % TileHeight == 0);

        for (int i = 0; i < BorderHeight; i++) {
            for (int j = 0; j < BorderWidth; j++) {
                auto pixelX = x + borderOffset.x + (j * TileWidth);
                auto pixelY = y + borderOffset.y + (i * TileHeight);
                PutTile(pixelX, pixelY, Derived::border[i][j]);
            }
        }
    }

    void
    DrawText(int textIndex, int x, int y) {
        assert(textIndex < TextCount);
        assert(x % DisplayType::OffsetPerBG == 0);
        assert(y % TileHeight == 0);

        auto pixelY = y + textOffset.y;
        for (int i = 0; i < TextGlyphCount; i++) {
            auto pixelX = x + textOffset.x + (i * TileWidth);
            PutGlyph(pixelX, pixelY, Derived::text[textIndex][i]);
        }
    }

  protected:
    void
    PutTile(int16_t x, int16_t y, FontType tile) const {
        assert(x % DisplayType::OffsetPerBG == 0);
        assert(y % TileHeight == 0);
        auto _idx =
            getLayerIndex<DisplayType::TileBGNum, DisplayType::OffsetPerBG>(x);
        layers[_idx].Put(x / TileWidth, y / TileHeight, tile);
    }

    void
    PutGlyph(int16_t x, int16_t y, const Glyph &glyph) const {
        assert(x % DisplayType::OffsetPerBG == 0);
        assert(y % TileHeight == 0);
        auto _idx =
            getLayerIndex<DisplayType::TileBGNum, DisplayType::OffsetPerBG>(x);
        layers[_idx].PutGlyph(x / TileWidth, y / TileHeight, glyph);
    }

    TileLayer<DisplayType> *layers;

    Point borderOffset;
    Point textOffset;
};

constexpr int SignTextCount = 2;
constexpr int SignTextGlyphCount = 1;
constexpr int SignBorderWidth = 4;
constexpr int SignBorderHeight = 4;
class NumberSignManager
    : public DrawerManager<NumberSignManager, SubDisplay, SignBorderWidth,
                           SignBorderHeight, SignTextCount,
                           SignTextGlyphCount> {
  public:
    friend class DrawerManager<NumberSignManager, SubDisplay, SignBorderWidth,
                               SignBorderHeight, SignTextCount,
                               SignTextGlyphCount>;

    static constexpr FontType border[SignBorderHeight][SignBorderWidth] = {
        {1, 0, 0, 7},
        {1, 0, 0, 7},
        {2, 0, 0, 6},
        {3, 8, 4, 5},
    };

    static constexpr Glyph text[SignTextCount][SignTextGlyphCount] = {
        {Glyph(35, 36)},                           // S
        {Glyph(16, 9, false, false, false, true)}, // U
    };

    using Base =
        DrawerManager<NumberSignManager, SubDisplay, SignBorderWidth,
                      SignBorderHeight, SignTextCount, SignTextGlyphCount>;

    NumberSignManager(TileLayer<SubDisplay> *layers, Point borderOffset,
                      Point textOffset)
        : Base(layers, borderOffset, textOffset) {}

    void
    DrawText(NumberSign sign, int x, int y) {
        int textIndex = [sign]() {
            switch (sign) {
            case Signed:
                return 0;
            case Unsigned:
                return 1;
            default:
                assert(false && "invalid sign");
                return 0;
            }
        }();
        Base::DrawText(textIndex, x, y);
    }
};

constexpr int WidthTextCount = 4;
constexpr int WidthTextGlyphCount = 5;
constexpr int WidthBorderWidth = 8;
constexpr int WidthBorderHeight = 4;

class NumberWidthManager
    : public DrawerManager<NumberWidthManager, SubDisplay, WidthBorderWidth,
                           WidthBorderHeight, WidthTextCount,
                           WidthTextGlyphCount> {
  public:
    friend class DrawerManager<NumberWidthManager, SubDisplay, WidthBorderWidth,
                               WidthBorderHeight, WidthTextCount,
                               WidthTextGlyphCount>;

    static constexpr auto BgType = BgType_Text4bpp;
    static constexpr auto BgSize = BgSize_T_256x256;

    static constexpr FontType border[WidthBorderHeight][WidthBorderWidth] = {
        {1, 0, 0, 0, 0, 0, 0, 7},
        {1, 0, 0, 0, 0, 0, 0, 7},
        {2, 0, 0, 0, 0, 0, 0, 6},
        {3, 8, 8, 8, 8, 8, 4, 5},
    };
    static constexpr Glyph text[WidthTextCount][WidthTextGlyphCount] = {
        {
            Glyph(9, 10),
            Glyph(11, 12),
            GlyphOY(9, 9),
            Glyph(14, 13),
            GlyphOY(15, 15),
        }, // QWORD
        {
            GlyphOY(15, 15),
            Glyph(11, 12),
            GlyphOY(9, 9),
            Glyph(14, 13),
            GlyphOY(15, 15),
        }, // DWORD
        {
            Glyph(17, 18),
            Glyph(19, 20),
            Glyph(22, 21),
            Glyph(23, 24),
            GlyphOY(25, 25),
        }, // WORD
        {
            Glyph(27, 26),
            Glyph(28, 29),
            Glyph(31, 30),
            Glyph(32, 26),
            GlyphOY(34, 33),
        }, // BYTE
    };

    using Base =
        DrawerManager<NumberWidthManager, SubDisplay, WidthBorderWidth,
                      WidthBorderHeight, WidthTextCount, WidthTextGlyphCount>;

    NumberWidthManager(TileLayer<SubDisplay> *layers, Point borderOffset,
                       Point textOffset)
        : Base(layers, borderOffset, textOffset) {}

    void
    DrawText(NumberWidth width, int x, int y) {
        int textIndex = [width]() {
            switch (width) {
            case QWord:
                return 0;
            case DWord:
                return 1;
            case Word:
                return 2;
            case Byte:
                return 3;
            default:
                assert(false && "invalid width");
                return 0;
            }
        }();
        Base::DrawText(textIndex, x, y);
    }
};

class MainDisplay;
class SubDisplay;

template <typename DisplayType>
class Display : private NonCopyable {
  public:
    Display(void) : sm() {}

    void
    SetBackdrop(uint16_t color) const {
        if constexpr (std::is_same_v<DisplayType, MainDisplay>) {
            setBackdropColor(color);
        } else if constexpr (std::is_same_v<DisplayType, SubDisplay>) {
            setBackdropColorSub(color);
        } else {
            // should never reach here
        }
    }

    void
    SetBackdrop(uint8_t r, uint8_t g, uint8_t b) const {
        uint8_t _r = r & 31;
        uint8_t _g = g & 31;
        uint8_t _b = b & 31;
        uint16_t color = RGB15(_r, _g, _b);
        SetBackdrop(color);
    }

    void
    UpdateSprites(void) {
        sm.Update();
    }

  protected:
    using SpriteManagerType =
        std::conditional_t<std::is_same_v<DisplayType, MainDisplay>,
                           MainSpriteManager, SubSpriteManager>;
    SpriteManagerType sm;
};

/**
 * @brief The main view of the calculator, which is displayed on the top
 * screen.
 *
 */
class MainDisplay : public Display<MainDisplay> {
  public:
    MainDisplay(void);

    /**
     * @brief Print a glyph at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param glyph The glyph to print
     */
    void PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const;

    /**
     * @brief Print a tile at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param tile The tile to print
     */
    void PutTile(int16_t x, int16_t y, FontType tile, bool hFlip = false,
                 bool vFlip = false) const;

    /**
     * @brief Print a line of glyphs.
     *
     * Walks layers by a fixed step instead of re-deriving the layer index from
     * pixel X on every glyph. For CharWidth 6, layerStep is 3 (0→3→2→1…);
     * for CharWidth 8, layerStep is 0 so the same layer is reused.
     *
     * @tparam GlyphIterable An iterable type that contains Glyphs
     * @param glyphRange The range of glyphs to print
     * @param start The starting position of the line
     */
    template <typename GlyphIterable>
    void
    PrintLine(const GlyphIterable &glyphRange, const Point &start) const {
        constexpr auto charWidth = GlyphIterable::CharWidth;
        // How many OffsetPerBG strips each glyph advances; 0 when charWidth is
        // a multiple of TileWidth (always the same layer).
        constexpr int layerStep = (charWidth / OffsetPerBG) & (TileBGNum - 1);

        const uint8_t tileY =
            static_cast<uint8_t>(static_cast<unsigned>(start.y) / TileHeight);
        int16_t px = start.x;
        auto tileX =
            static_cast<uint8_t>(static_cast<unsigned>(px) / TileWidth);
        int layerIdx = getLayerIndex<TileBGNum, OffsetPerBG>(px);

        for (const auto &glyph : glyphRange) {
            layers[layerIdx].PutGlyph(tileX, tileY, glyph);
            px += charWidth;
            tileX = static_cast<uint8_t>(static_cast<unsigned>(px) / TileWidth);
            if constexpr (layerStep != 0) {
                layerIdx = (layerIdx + layerStep) & (TileBGNum - 1);
            }
        }
    }

    /**
     * @brief Clear glyphs from @p start to the right edge of the screen.
     *
     * Same layer-rotation scheme as PrintLine; writes FontEmpty directly into
     * each layer's map instead of calling PutTile per cell.
     *
     * @param start Leftmost pixel of the region to clear
     * @param charWidth Glyph pitch in pixels (typically 6 or 8)
     * @param underline If true, also clear the underline row (3 tile rows)
     */
    void ClearLine(const Point &start, int charWidth,
                   bool underline = false) const;

    /**
     * @brief Clear every OffsetPerBG strip in a horizontal band.
     *
     * Unlike ClearLine (which steps by charWidth), this hits all four tile
     * layers so residue from sub-glyph scroll offsets cannot remain.
     *
     * @param start Leftmost pixel of the band
     * @param pixelWidth Width in pixels (must be a multiple of OffsetPerBG)
     * @param underline If true, also clear the underline row (3 tile rows)
     */
    void ClearLineBand(const Point &start, int pixelWidth,
                       bool underline = false) const;

    static constexpr int Bpp = 4;
    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;
    static constexpr int TileSize = TileWidth * TileHeight;
    /**
     * @brief all 4 layers are text layers
     *
     */
    static constexpr auto VideoMode =
        MODE_0_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT;
    static constexpr int TileBGNum = 4;
    static constexpr int OffsetPerBG = TileWidth / TileBGNum;
    static constexpr int MaxTileNum = 256;

    /**
     * @brief Global scroll offset X for all layers.
     *
     */
    static constexpr uint16_t offsetX = 0;

    /**
     * @brief Global scroll offset Y for all layers.
     *
     */
    static constexpr uint16_t offsetY = 0;

  private:
    TileLayer<MainDisplay> layers[TileBGNum];
};

/**
 * @brief The sub view of the calculator, which is displayed on the bottom
 * screen.
 *
 */
class SubDisplay : public Display<SubDisplay> {
  public:
    SubDisplay(void);

    /**
     * @brief Print a glyph at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param glyph The glyph to print
     */
    void PrintGlyph(int16_t x, int16_t y, const Glyph &glyph) const;

    /**
     * @brief Print a tile at the given position.
     *
     * @param x The x-coordinate of the position
     * @param y The y-coordinate of the position
     * @param tile The tile to print
     */
    void PutTile(int16_t x, int16_t y, FontType tile, bool hFlip = false,
                 bool vFlip = false) const;

    /**
     * @brief Copy the font palette and set the backdrop color.
     *
     */
    void InitializePalette(void);

    /**
     * @brief Pre-decoded subscreen background images.
     *
     * Bitmaps are LZ77-decompressed into main RAM at construction time so that
     * view switches only need a dmaCopy into VRAM.
     */
    enum class Image : uint8_t {
        InputImage,
        BinaryImage,
    };

    /**
     * @brief Setup the view with a pre-decoded bitmap and its palette.
     *
     * @param image Which preloaded subscreen image to copy into VRAM.
     */
    void SetupView(Image image);

    void DisableButton(int index);
    void EnableButton(int index);
    void SelectButton(int index);
    void SetButtonSignColor(int index, uint16_t color);

    void UpdateWidthDrawer(NumberWidth width);
    void UpdateSignDrawer(NumberSign sign);

    void DrawDrawerBorders(void);

    Sprite<SubDisplay> *AddSprite(Point position, int priority = 0,
                                  bool hFlip = false, bool vFlip = false);
    void ResetSprites(void);

    void CleanLayers(void);

    void ShowVerStr(void);

    static constexpr auto VideoMode =
        MODE_3_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT;
    static constexpr int TileBGNum = 2;
    static constexpr int MaxTileNum = 320;

    static constexpr int ColorCount = 5;
    static constexpr int MaxColorCount = 256;

    static constexpr int Bpp = 4;
    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;
    static constexpr int TileSize = TileWidth * TileHeight;
    static constexpr int OffsetPerBG = TileWidth / TileBGNum;

    /**
     * @brief Global scroll offset X for all layers.
     *
     */
    static constexpr uint16_t offsetX = 0;

    /**
     * @brief Global scroll offset Y for all layers.
     *
     */
    static constexpr uint16_t offsetY = 0;

  private:
    BmpLayer<SubDisplay> bmpLayer;
    TileLayer<SubDisplay> tileLayers[TileBGNum];
    NumberWidthManager widthManager;
    NumberSignManager signManager;

    uint16_t *enabledPalette;
    uint16_t *disabledPalette;
    uint16_t *selectedPalette;
};

}; // namespace HexCalc
