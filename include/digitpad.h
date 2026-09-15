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
#include "viewmodel.h"

namespace HexCalc {

class DigitFocus {
  public:
    DigitFocus(SubDisplay &display);

    void SetPosition(Point newPos);

    bool
    Visible(void) const {
        return visible;
    }

    void Show(void);

    void Hide(void);

  private:
    SubDisplay &display;

    bool visible;

    Sprite<SubDisplay> *sprites[4];
};

class DigitPad : public NonCopyable {
  public:
    DigitPad(SubDisplay &display, ViewModel &viewModel);

    void DrawDigits(void);
    void Setup(void);

    void MoveFocus(Direction dir);

    int
    GetFocusedIndex(void) const {
        return 64 - 1 - (focus.x + (focus.y * colNum));
    }

  private:
    friend class DigitFocus;

    static constexpr size_t colNum = 16;
    static constexpr size_t rowNum = 4;

    static_assert(colNum * rowNum == 64, "DigitPad must have 64 digits");

    static constexpr size_t offsetX = 12;
    static constexpr size_t offsetY = 64;

    static constexpr size_t gapX = 12;
    static constexpr size_t lineHeight = 32;

    static constexpr size_t columnGap = 8;
    static constexpr size_t columnCount = 4;

    SubDisplay &display;
    ViewModel &vm;

    Point focus;

    DigitFocus digitFocus;
};

} // namespace HexCalc
