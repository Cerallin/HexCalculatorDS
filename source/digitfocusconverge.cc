/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationeffects.h"
#include "theme.h"
#include "viewhost.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

namespace {

constexpr Point kOutward[DigitFocus::CornerCount] = {
    Point(-1, -1),
    Point(1, -1),
    Point(-1, 1),
    Point(1, 1),
};

} // namespace

DigitFocusConverge::DigitFocusConverge(EditorView &editorView,
                                       DigitFocusAnimState &state)
    : digitPad(editorView.GetDigitPad()), digitFocus(digitPad.GetDigitFocus()),
      state(state), phase(0), toPx(0, 0),
      startCorners{Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)},
      endCorners{Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)} {}

void
DigitFocusConverge::AfterHandle(const Event &e, AnimationGate &gate) {
    if (e.type == EventType::InputViewChangedEvent) {
        if (active) {
            gate.Cancel(AnimationChannel::NonBlocking);
        }
        active = false;
        phase = 0;
        state.Clear();
        return;
    } else if ((e.type == EventType::MoveFocusEvent) ||
               (e.type == EventType::FlipBitEvent)) {

        if (!digitPad.HasFocus() || state.HasVisual()) {
            return;
        }

        const Point cell = digitPad.FocusCell();
        toPx = DigitPad::CellToFocusPixel(cell);

        Point locked[DigitFocus::CornerCount] = {Point(0, 0), Point(1, 0),
                                                 Point(0, 7), Point(1, 7)};
        DigitFocus::LockedCornerOffsets(locked);
        for (int i = 0; i < DigitFocus::CornerCount; i++) {
            int16_t x = toPx.x + locked[i].x;
            int16_t y = toPx.y + locked[i].y;
            int16_t outwardX = kOutward[i].x * SpreadPx;
            int16_t outwardY = kOutward[i].y * SpreadPx;

            endCorners[i].x = x;
            endCorners[i].y = y;

            startCorners[i].x = x + outwardX;
            startCorners[i].y = y + outwardY;
        }

        digitFocus.SetSignColor(COLOR_COMMON_BG);
        digitFocus.SetCornerPositions(startCorners);
        digitFocus.Show();
        if (!start(gate)) {
            snapToLogical();
            return;
        }
        state.lastCell = cell;
    } else {
        // Do nothing
    }
}

bool
DigitFocusConverge::start(AnimationGate &gate) {
    phase = 0;
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &DigitFocusConverge::TickThunk,
                              &DigitFocusConverge::CancelThunk))) {
        active = false;
        return false;
    }
    active = true;
    return true;
}

void
DigitFocusConverge::Cancel(void) {
    active = false;
    phase = 0;
    if (digitPad.HasFocus()) {
        digitFocus.ResetSignColor();
    }
}

void
DigitFocusConverge::snapToLogical(void) {
    if (digitPad.HasFocus()) {
        digitPad.SnapFocusVisual();
        digitFocus.ResetSignColor();
        state.lastCell = digitPad.FocusCell();
    } else {
        digitFocus.Hide();
        state.Clear();
    }
}

bool
DigitFocusConverge::Tick(void) {
    const uint16_t t256 = static_cast<uint16_t>(
        (static_cast<uint32_t>(phase + 1) * 256) / Frames);
    const uint16_t eased = EaseInOutCubicBezier(t256);

    Point corners[DigitFocus::CornerCount] = {Point(0, 0), Point(0, 0),
                                              Point(0, 0), Point(0, 0)};
    for (int i = 0; i < DigitFocus::CornerCount; i++) {
        corners[i].x = LerpInt(startCorners[i].x, endCorners[i].x, eased);
        corners[i].y = LerpInt(startCorners[i].y, endCorners[i].y, eased);
    }
    digitFocus.SetCornerPositions(corners);
    digitFocus.SetSignColor(
        LerpRgb15(COLOR_COMMON_BG, COLOR_COMMON_BORDER, eased));

    phase++;
    if (phase >= Frames) {
        digitFocus.SetPixelPosition(toPx);
        digitFocus.ResetSignColor();
        active = false;
        return false;
    }
    return true;
}

bool
DigitFocusConverge::TickThunk(void *ctx) {
    return static_cast<DigitFocusConverge *>(ctx)->Tick();
}

void
DigitFocusConverge::CancelThunk(void *ctx) {
    static_cast<DigitFocusConverge *>(ctx)->Cancel();
}
