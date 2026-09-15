/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "digitpad.h"
#include "subFont.h"

using namespace HexCalc;

DigitFocus::DigitFocus(SubDisplay &display) : display(display), visible(false) {
    // Add sprites
    sprites[0] = display.AddSprite(Point(0, 0), 1, false, false);
    sprites[1] = display.AddSprite(Point(0, 0), 1, true, false);
    sprites[2] = display.AddSprite(Point(0, 0), 1, false, true);
    sprites[3] = display.AddSprite(Point(0, 0), 1, true, true);

    // Hide all sprites by default
    Hide();
}

void
DigitFocus::Show(void) {
    sprites[0]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[1]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[2]->SetTileOffset(DigitFocusTileOffset + 1);
    sprites[3]->SetTileOffset(DigitFocusTileOffset + 1);

    visible = true;
}

void
DigitFocus::Hide(void) {
    sprites[0]->SetTileOffset(0);
    sprites[1]->SetTileOffset(0);
    sprites[2]->SetTileOffset(0);
    sprites[3]->SetTileOffset(0);

    visible = false;
}

void
DigitFocus::SetPosition(Point newPos) {
    int offsetX = -1;
    int offsetY = 2;
    // Same layout as DigitPad::DrawDigits
    int x = DigitPad::offsetX + (newPos.x * DigitPad::gapX);
    x += (newPos.x / DigitPad::columnCount) * DigitPad::columnGap;
    int y = DigitPad::offsetY + (newPos.y * DigitPad::lineHeight);

    sprites[0]->SetPosition(x + offsetX + 0, y + offsetY + 0);
    sprites[1]->SetPosition(x + offsetX + 1, y + offsetY + 0);
    sprites[2]->SetPosition(x + offsetX + 0, y + offsetY + 7);
    sprites[3]->SetPosition(x + offsetX + 1, y + offsetY + 7);
}

DigitPad::DigitPad(SubDisplay &display, ViewModel &viewModel)
    : display(display), vm(viewModel), focus(-1, -1), digitFocus(display),
      handler(viewModel.Cmds()) {}

void
DigitPad::DrawDigits(void) {
    size_t glyphOffset = sizeof(subFontMap) / sizeof(subFontMap[0]);
    Glyph glyph1(0 + glyphOffset, 1 + glyphOffset);
    Glyph glyph0(3 + glyphOffset, 2 + glyphOffset);

    auto number = vm.GetValueDigits<64>(NumberBase::Binary);
    // TODO: check assembly code for optimization
    for (size_t i = 0; i < colNum; i++) {
        for (size_t j = 0; j < rowNum; j++) {
            size_t x = offsetX + (i * gapX);
            // Add a gap between every N columns
            x += (i / columnCount) * columnGap;

            size_t y = offsetY + (j * lineHeight);

            auto digit = number[64 - 1 - ((j * colNum) + i)];

            if (digit == Digit0) {
                display.PrintGlyph(x, y, glyph0);
            } else { // Digit1
                display.PrintGlyph(x, y, glyph1);
            }
        }
    }
}

void
DigitPad::Setup(void) {
    DrawDigits();

    // Reset focus
    focus = Point(-1, -1);

    // Hide focus sprites
    digitFocus.Hide();
}

void
DigitPad::RegisterDigitButtons(void) {
    // Register digit buttons
    for (size_t i = 0; i < colNum; i++) {
        for (size_t j = 0; j < rowNum; j++) {
            int16_t x = offsetX + (i * gapX) + ((i / columnCount) * columnGap);
            int16_t y = offsetY + (j * lineHeight);
            uint8_t width = 8;
            uint8_t height = 18;
            Area area(x, y, width, height);
            int bitIndex = 64 - 1 - static_cast<int>((j * colNum) + i);
            HEXCALC_GCC_UNUSED auto button = handler.RegisterButton(
                area, ButtonType::ButtonFlipBit, i, j, bitIndex);
        }
    }
}

void
DigitPad::MoveFocus(Direction dir) {
    if (focus.x < 0 && focus.y < 0) {
        // If no button is focused, set focus to the first button
        focus = Point(0, 0);
        // Show focus sprites
        digitFocus.Show();
    } else {
        focus = focus.NextPosition<colNum, rowNum>(dir);
    }

    debugf("Focus: (%d, %d)\n", focus.x, focus.y);

    digitFocus.SetPosition(focus);
}

void
DigitPad::SetFocus(int index) {
    int bitIndex = 64 - 1 - index;
    int x = bitIndex % colNum;
    int y = bitIndex / colNum;
    focus = Point(x, y);
    digitFocus.SetPosition(focus);
    // Show focus sprites
    digitFocus.Show();
}

void
DigitPad::Handle(const Point &touchPoint) {
    bool handled = handler.Handle(touchPoint);
    if (!handled) {
        digitFocus.Hide();
    }
}
