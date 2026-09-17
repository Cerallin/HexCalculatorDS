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
    static constexpr int CornerCount = 4;
    static constexpr int BaseOffsetX = -1;
    static constexpr int BaseOffsetY = 2;

    DigitFocus(SubDisplay &display);

    /**
     * @brief Place the focus frame at a digit cell (locked corner layout).
     */
    void SetPosition(Point cell);

    /**
     * @brief Place the focus frame by focus-base pixel (glyph origin + base
     *        offset). Corners use the locked relative offsets.
     */
    void SetPixelPosition(int x, int y);

    void SetPixelPosition(const Point &pos);

    Point GetPixelPosition(void) const;

    /**
     * @brief Place each corner sprite independently (converge / reticle lock).
     */
    void SetCornerPositions(const Point corners[CornerCount]);

    /**
     * @brief Relative offsets of the four corner sprites from the focus base.
     */
    static void LockedCornerOffsets(Point out[CornerCount]);

    void Show(void);

    void Hide(void);

    /**
     * @brief Set the focus-frame sign color (palette bank entry used by tiles).
     */
    void SetSignColor(uint16_t color);

    /**
     * @brief Restore sign color to COLOR_COMMON_BORDER.
     */
    void ResetSignColor(void);

  private:
    SubDisplay &display;

    Sprite<SubDisplay> *sprites[CornerCount];
    Point pixelPos;

    void applyLockedCorners(void);
};

class DigitPad : public NonCopyable {
  public:
    DigitPad(SubDisplay &display, ViewModel &viewModel);

    void DrawDigits(void);
    void Setup(void);
    void Teardown(void);
    void RegisterDigitButtons(void);

    /**
     * @brief Enable/disable digit buttons for the current NumberWidth and clear
     * focus if it sits on a disabled bit.
     */
    void HandleWidthChange(void);

    void MoveFocus(Direction dir);

    int
    GetFocus(void) const {
        return 64 - 1 - (focus.x + (focus.y * colNum));
    }

    void SetFocus(int index);

    void HandleButtons(const Point &touchPoint);

    bool
    HasFocus(void) const {
        return (focus.x >= 0) && (focus.y >= 0);
    }

    Point
    FocusCell(void) const {
        return focus;
    }

    DigitFocus &
    GetDigitFocus(void) {
        return digitFocus;
    }

    /**
     * @brief Glyph origin pixel for a digit cell (same layout as DrawDigits).
     */
    static Point CellToPixel(Point cell);

    /**
     * @brief Focus-base pixel for a digit cell (CellToPixel + base offset).
     */
    static Point CellToFocusPixel(Point cell);

    /**
     * @brief Snap sprites to the current logical focus (locked layout).
     */
    void SnapFocusVisual(void);

    /**
     * @brief True if the most recent MoveFocus was not a visual neighbor step
     *        (row/column edge fold or width modulo wrap).
     */
    bool
    LastFocusWrapped(void) const {
        return focusWrapped;
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
    bool focusWrapped;

    DigitFocus digitFocus;

    TouchScreenHandler<colNum, rowNum> handler;

    bool isBitActive(int bitIndex) const;
    Point bitToPoint(int bitIndex) const;
    int nextBitIndex(int bitIndex, Direction dir, int width) const;
    bool isWrapMove(int fromBit, Direction dir, int width) const;
    void updateButtons(void);
};

} // namespace HexCalc
