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
template <class Derived, typename DisplayType, int BorderWidth,
          int BorderHeight, int TextCount, int TextGlyphCount>
class DrawerManager {
  public:
    constexpr DrawerManager(TileLayer<DisplayType> &borderLayer,
                            TileLayer<DisplayType> &textLayer,
                            Point borderOffset, Point textOffset)
        : borderLayer(borderLayer), textLayer(textLayer),
          borderOffset(borderOffset), textOffset(textOffset) {}

    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;

    void
    DrawBorders(int x, int y) {
        assert(x % TileWidth == 0);
        assert(y % TileHeight == 0);

        auto offsetX = borderOffset.x;
        auto offsetY = borderOffset.y;

        for (int i = 0; i < BorderHeight; i++) {
            for (int j = 0; j < BorderWidth; j++) {
                auto _x = (x + offsetX) / TileWidth + j;
                auto _y = (y + offsetY) / TileHeight + i;

                borderLayer.Put(_x, _y, Derived::border[i][j]);
            }
        }
    }

    void
    DrawText(int textIndex, int x, int y) {
        assert(textIndex < TextCount);
        assert(x % TileWidth == 0);
        assert(y % TileHeight == 0);

        auto offsetX = textOffset.x;
        auto offsetY = textOffset.y;

        auto _x = (x + offsetX) / TileWidth;
        auto _y = (y + offsetY) / TileHeight;

        for (int i = 0; i < TextGlyphCount; i++) {
            textLayer.PutGlyph(_x + i, _y, Derived::text[textIndex][i]);
        }
    }

  protected:
    TileLayer<DisplayType> &borderLayer;
    TileLayer<DisplayType> &textLayer;

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

    NumberSignManager(TileLayer<SubDisplay> &borderLayer,
                      TileLayer<SubDisplay> &textLayer, Point borderOffset,
                      Point textOffset)
        : Base(borderLayer, textLayer, borderOffset, textOffset) {}

    void
    DrawText(NumberSign sign, int x, int y) {
        int textIndex = 0;
        switch (sign) {
        case Signed:
            textIndex = 0;
            break;
        case Unsigned:
            textIndex = 1;
            break;
        default:
            assert(false && "invalid sign");
        }
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

    NumberWidthManager(TileLayer<SubDisplay> &borderLayer,
                       TileLayer<SubDisplay> &textLayer, Point borderOffset,
                       Point textOffset)
        : Base(borderLayer, textLayer, borderOffset, textOffset) {}

    void
    DrawText(NumberWidth width, int x, int y) {
        int textIndex = 0;
        switch (width) {
        case QWord:
            textIndex = 0;
            break;
        case DWord:
            textIndex = 1;
            break;
        case Word:
            textIndex = 2;
            break;
        case Byte:
            textIndex = 3;
            break;
        default:
            assert(false && "invalid width");
        }
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
    SpriteManager<DisplayType> sm;
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
     * @tparam Iterable An iterable type that contains Glyphs
     * @param glyphRange The range of glyphs to print
     * @param skip The number of glyphs to skip at the beginning of the line
     * @param start The starting position of the line
     * @param charWidth The width of each character in pixels
     */
    template <typename GlyphIterable>
    void
    PrintLine(const GlyphIterable &glyphRange, Point &start) const {
        constexpr auto charWidth = GlyphIterable::CharWidth;
        auto x = start.x;
        auto y = start.y;
        // print glyphs
        size_t index = 0;
        for (const auto &glyph : glyphRange) {
            auto glyphX = start.x + (index * charWidth);
            this->PrintGlyph(glyphX, y, glyph);
            index++;
        }
    }

    void ClearLine(Point &start, int charWidth, bool underline = false) const;

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

    void DisableButton(int index);
    void EnableButton(int index);
    void SelectButton(int index);

    void UpdateWidthDrawer(NumberWidth width);
    void UpdateSignDrawer(NumberSign sign);

    Sprite<SubDisplay> *AddSprite(Point position, int priority = 0);

    // FIXME sprites
    static constexpr auto VideoMode =
        MODE_3_2D | DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT;
    static constexpr int TileBGNum = 3;
    static constexpr int MaxTileNum = 320;

    static constexpr int ColorCount = 5;
    static constexpr int MaxColorCount = 256;

    static constexpr int Bpp = 4;
    static constexpr int TileWidth = 8;
    static constexpr int TileHeight = 8;
    static constexpr int TileSize = TileWidth * TileHeight;

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
