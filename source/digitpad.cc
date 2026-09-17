/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "digitpad.h"
#include "subFont.h"
#include "theme.h"

using namespace HexCalc;

DigitFocus::DigitFocus(SubDisplay &display) : display(display), pixelPos(0, 0) {
    // Add sprites on a dedicated palette bank for independent sign-color lerp.
    sprites[0] = display.AddSprite(Point(0, 0), 1, false, false);
    sprites[1] = display.AddSprite(Point(0, 0), 1, true, false);
    sprites[2] = display.AddSprite(Point(0, 0), 1, false, true);
    sprites[3] = display.AddSprite(Point(0, 0), 1, true, true);

    for (int i = 0; i < CornerCount; ++i) {
        sprites[i]->SetPalette(DigitFocusPaletteBank);
    }
    ResetSignColor();

    // Hide all sprites by default
    Hide();
}

void
DigitFocus::Show(void) {
    sprites[0]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[1]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[2]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[3]->SetTileOffset(DigitFocusTileOffset + 1);
}

void
DigitFocus::Hide(void) {
    sprites[0]->SetTileOffset(0);
    sprites[1]->SetTileOffset(0);
    sprites[2]->SetTileOffset(0);
    sprites[3]->SetTileOffset(0);
}

void
DigitFocus::SetSignColor(uint16_t color) {
    display.SetSpritePaletteColor(DigitFocusPaletteBank,
                                  DigitFocusSignColorIndex, color);
}

void
DigitFocus::ResetSignColor(void) {
    SetSignColor(COLOR_COMMON_BORDER);
}

void
DigitFocus::LockedCornerOffsets(Point out[CornerCount]) {
    out[0] = Point(0, 0);
    out[1] = Point(1, 0);
    out[2] = Point(0, 7);
    out[3] = Point(1, 7);
}

void
DigitFocus::applyLockedCorners(void) {
    Point offsets[CornerCount] = {Point(0, 0), Point(1, 0), Point(0, 7),
                                  Point(1, 7)};
    for (int i = 0; i < CornerCount; i++) {
        sprites[i]->SetPosition(pixelPos.x + offsets[i].x,
                                pixelPos.y + offsets[i].y);
    }
}

void
DigitFocus::SetPixelPosition(int x, int y) {
    pixelPos = Point(x, y);
    applyLockedCorners();
}

void
DigitFocus::SetPixelPosition(const Point &pos) {
    SetPixelPosition(pos.x, pos.y);
}

Point
DigitFocus::GetPixelPosition(void) const {
    return pixelPos;
}

void
DigitFocus::SetCornerPositions(const Point corners[CornerCount]) {
    for (int i = 0; i < CornerCount; i++) {
        sprites[i]->SetPosition(corners[i]);
    }
    // Keep pixelPos as the locked base implied by corner 0 when possible.
    pixelPos = Point(corners[0].x, corners[0].y);
}

void
DigitFocus::SetPosition(Point newPos) {
    const Point base = DigitPad::CellToFocusPixel(newPos);
    SetPixelPosition(base.x, base.y);
}

Point
DigitPad::CellToPixel(Point cell) {
    const int x = static_cast<int>(offsetX + (cell.x * gapX) +
                                   (cell.x / columnCount) * columnGap);
    const int y = static_cast<int>(offsetY + (cell.y * lineHeight));
    return Point(x, y);
}

Point
DigitPad::CellToFocusPixel(Point cell) {
    const Point glyph = CellToPixel(cell);
    return Point(glyph.x + DigitFocus::BaseOffsetX,
                 glyph.y + DigitFocus::BaseOffsetY);
}

void
DigitPad::SnapFocusVisual(void) {
    if (!HasFocus()) {
        digitFocus.Hide();
        return;
    }
    digitFocus.SetPosition(focus);
    digitFocus.Show();
}

DigitPad::DigitPad(SubDisplay &display, ViewModel &viewModel)
    : display(display), vm(viewModel), focus(-1, -1), digitFocus(display),
      handler(viewModel.Cmds()) {}

bool
DigitPad::isBitActive(int bitIndex) const {
    return bitIndex < static_cast<int>(vm.GetNumberWidth());
}

Point
DigitPad::bitToPoint(int bitIndex) const {
    int index = 64 - 1 - bitIndex;
    return Point(index % colNum, index / colNum);
}

void
DigitPad::DrawDigits(void) {
    constexpr size_t glyphOffset = sizeof(subFontMap) / sizeof(subFontMap[0]);
    constexpr Glyph glyph1(0 + glyphOffset, 1 + glyphOffset);
    constexpr Glyph glyph0(3 + glyphOffset, 2 + glyphOffset);
    constexpr Glyph glyph0Disabled(48 + glyphOffset, 49 + glyphOffset);

    // Walk bits MSB-first in row-major order: (i,j) -> bit 63-(j*colNum+i)
    auto value = vm.GetRawValue();
    for (size_t j = 0; j < rowNum; j++) {
        size_t y = offsetY + (j * lineHeight);
        for (size_t i = 0; i < colNum; i++) {
            size_t x = offsetX + (i * gapX) + (i / columnCount) * columnGap;
            int bitIndex = 64 - 1 - static_cast<int>((j * colNum) + i);

            if (!isBitActive(bitIndex)) {
                display.PrintGlyph(x, y, glyph0Disabled);
            } else {
                bool bitSet = (value & (NumberDataType(1) << 63)) != 0;
                if (bitSet) {
                    display.PrintGlyph(x, y, glyph1);
                } else {
                    display.PrintGlyph(x, y, glyph0);
                }
            }
            value <<= 1;
        }
    }
}

void
DigitPad::updateButtons(void) {
    for (size_t i = 0; i < handler.Size(); i++) {
        auto &button = handler.GetButton(i);
        if (isBitActive(button.Index())) {
            button.Enable();
        } else {
            button.Disable();
        }
    }
}

void
DigitPad::HandleWidthChange(void) {
    updateButtons();

    // Drop focus if the highlighted bit is no longer in range
    if ((focus.x >= 0) && (focus.y >= 0) && !isBitActive(GetFocus())) {
        focus = Point(-1, -1);
        digitFocus.Hide();
    }
}

void
DigitPad::Setup(void) {
    DrawDigits();
    updateButtons();

    // Reset focus
    focus = Point(-1, -1);

    // Hide focus sprites
    digitFocus.Hide();
}

void
DigitPad::Teardown(void) {
    focus = Point(-1, -1);
    digitFocus.Hide();
}

void
DigitPad::RegisterDigitButtons(void) {
    // Register digit buttons
    for (size_t i = 0; i < colNum; i++) {
        for (size_t j = 0; j < rowNum; j++) {
            int16_t x = offsetX + (i * gapX) + ((i / columnCount) * columnGap);
            int16_t y = offsetY + (j * lineHeight);
            constexpr uint8_t width = 8;
            constexpr uint8_t height = 18;
            Area area(x, y, width, height);
            int bitIndex = 64 - 1 - static_cast<int>((j * colNum) + i);
            HEXCALC_GCC_UNUSED auto button = handler.RegisterButton(
                area, ButtonType::ButtonFlipBit, i, j, bitIndex);
        }
    }
}

int
DigitPad::nextBitIndex(int bitIndex, Direction dir, int width) const {
    // Navigate in active bit-index space. Left/right must not wrap across the
    // full 16-wide row (most cells are disabled when width < 64).
    switch (dir) {
    case Direction::DirLeft:
        // Visually left is the next higher bit
        return (bitIndex + 1) % width;
    case Direction::DirRight:
        return (bitIndex - 1 + width) % width;
    case Direction::DirUp: {
        const int next = bitIndex + static_cast<int>(colNum);
        if (next < width) {
            return next;
        }
        // Same column: wrap to the lowest active bit
        const int col = bitToPoint(bitIndex).x;
        for (int b = 0; b < width; ++b) {
            if (bitToPoint(b).x == col) {
                return b;
            }
        }
        return bitIndex;
    }
    case Direction::DirDown: {
        const int next = bitIndex - static_cast<int>(colNum);
        if (next >= 0) {
            return next;
        }
        // Same column: wrap to the highest active bit
        const int col = bitToPoint(bitIndex).x;
        for (int b = width - 1; b >= 0; --b) {
            if (bitToPoint(b).x == col) {
                return b;
            }
        }
        return bitIndex;
    }
    default:
        return bitIndex;
    }
}

void
DigitPad::MoveFocus(Direction dir) {
    int width = static_cast<int>(vm.GetNumberWidth());

    if (focus.x < 0 && focus.y < 0) {
        // Prefer the MSB among bits that are active for the current width
        focus = bitToPoint(width - 1);
    } else {
        focus = bitToPoint(nextBitIndex(GetFocus(), dir, width));
    }

    debugf("Focus: (%d, %d)\n", focus.x, focus.y);

    // Final visual state lives here so focus still works without animation
    // effects; effects may overwrite pixels in AfterHandle before OAM flush
    SnapFocusVisual();
}

void
DigitPad::SetFocus(int index) {
    if (!isBitActive(index)) {
        return;
    }

    focus = bitToPoint(index);
    SnapFocusVisual();
}

void
DigitPad::HandleButtons(const Point &touchPoint) {
    bool handled = handler.Handle(touchPoint);
    if (!handled) {
        focus = Point(-1, -1);
        digitFocus.Hide();
    }
}
