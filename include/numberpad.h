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

class NumberPad : public NonCopyable {
  public:
    NumberPad(SubDisplay &display, ViewModel &viewModel);

    void DrawBits(void);
    void Reset(void);

    void MoveFocus(Direction dir);

  private:
    static constexpr size_t colNum = 16;
    static constexpr size_t rowNum = 4;

    static_assert(colNum * rowNum == 64, "NumberPad must have 64 digits");

    static constexpr size_t offsetX = 12;
    static constexpr size_t offsetY = 64;

    static constexpr size_t gapX = 12;
    static constexpr size_t lineHeight = 32;

    static constexpr size_t columnGap = 8;
    static constexpr size_t columnCount = 4;

    SubDisplay &display;
    ViewModel &vm;

    Point focus;
};

} // namespace HexCalc
