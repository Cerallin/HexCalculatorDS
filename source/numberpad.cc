/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "numberpad.h"
#include "subFont.h"

using namespace HexCalc;

NumberPad::NumberPad(SubDisplay &display, ViewModel &viewModel)
    : display(display), vm(viewModel), focus(-1, -1) {}

void
NumberPad::DrawBits(void) {
    size_t glyphOffset = sizeof(subFontMap) / sizeof(subFontMap[0]);
    Glyph glyph1(0 + glyphOffset, 1 + glyphOffset);
    Glyph glyph0(3 + glyphOffset, 2 + glyphOffset);

    auto number = vm.GetValueDigits<64>(NumberBase::Binary);
    for (size_t i = 0; i < colNum; i++) {
        for (size_t j = 0; j < rowNum; j++) {
            size_t x = offsetX + (i * gapX);
            // Add a gap between every N columns
            x += (i / columnCount) * columnGap;

            size_t y = offsetY + (j * lineHeight);

            auto digit = number[(i * rowNum) + j];

            if (digit == Digit0) {
                display.PrintGlyph(x, y, glyph0);
            } else { // Digit1
                display.PrintGlyph(x, y, glyph1);
            }
        }
    }
}

void
NumberPad::Reset(void) {
    DrawBits();
    // reset focus
    focus = Point(-1, -1);
}

void
NumberPad::MoveFocus(Direction dir) {
    if (focus.x < 0 && focus.y < 0) {
        // If no button is focused, set focus to the first button
        focus = Point(0, 0);
    } else {
        focus = focus.NextPosition<colNum, rowNum>(dir);
    }
}
