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

DigitFocusDiverge::DigitFocusDiverge(EditorView &editorView,
                                     DigitFocusAnimState &state)
    : digitPad(editorView.GetDigitPad()), digitFocus(digitPad.GetDigitFocus()),
      state(state), phaseKind(Phase::Diverge), wrapFollow(false), phase(0),
      toCell(-1, -1), toPx(0, 0),
      startCorners{Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)},
      endCorners{Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)} {}

void
DigitFocusDiverge::setupDivergeCorners(Point basePx) {
    Point locked[DigitFocus::CornerCount] = {Point(0, 0), Point(1, 0),
                                             Point(0, 7), Point(1, 7)};
    DigitFocus::LockedCornerOffsets(locked);
    for (int i = 0; i < DigitFocus::CornerCount; i++) {
        const int16_t x = basePx.x + locked[i].x;
        const int16_t y = basePx.y + locked[i].y;
        startCorners[i].x = x;
        startCorners[i].y = y;
        endCorners[i].x = x + kOutward[i].x * SpreadPx;
        endCorners[i].y = y + kOutward[i].y * SpreadPx;
    }
}

void
DigitFocusDiverge::setupConvergeCorners(Point basePx) {
    Point locked[DigitFocus::CornerCount] = {Point(0, 0), Point(1, 0),
                                             Point(0, 7), Point(1, 7)};
    DigitFocus::LockedCornerOffsets(locked);
    for (int i = 0; i < DigitFocus::CornerCount; i++) {
        const int16_t x = basePx.x + locked[i].x;
        const int16_t y = basePx.y + locked[i].y;
        endCorners[i].x = x;
        endCorners[i].y = y;
        startCorners[i].x = x + kOutward[i].x * SpreadPx;
        startCorners[i].y = y + kOutward[i].y * SpreadPx;
    }
}

void
DigitFocusDiverge::AfterHandle(const Event &e, AnimationGate &gate) {
    if (e.type == EventType::InputViewChangedEvent) {
        clearVisual(gate);
        return;
    } else if ((e.type == EventType::TouchScreenEvent) ||
               (e.type == EventType::UpdateWidthEvent)) {
        if (digitPad.HasFocus() || !state.HasVisual()) {
            return;
        }

        // DigitPad already Hid(); pixel cache is intact — revive and diverge.
        wrapFollow = false;
        phaseKind = Phase::Diverge;
        setupDivergeCorners(digitFocus.GetPixelPosition());

        // Mark visual focus gone so a re-focus mid-animation starts Converge.
        state.Clear();
        digitFocus.SetSignColor(COLOR_COMMON_BORDER);
        digitFocus.SetCornerPositions(startCorners);
        digitFocus.Show();
        if (!start(gate)) {
            digitFocus.Hide();
            digitFocus.ResetSignColor();
        }
    }
}

bool
DigitFocusDiverge::StartWrap(Point fromCell, Point toCellArg,
                             AnimationGate &gate) {
    wrapFollow = true;
    phaseKind = Phase::Diverge;
    toCell = toCellArg;
    toPx = DigitPad::CellToFocusPixel(toCellArg);

    setupDivergeCorners(DigitPad::CellToFocusPixel(fromCell));
    state.Clear();
    digitFocus.SetSignColor(COLOR_COMMON_BORDER);
    digitFocus.SetCornerPositions(startCorners);
    digitFocus.Show();
    if (!start(gate)) {
        digitPad.SnapFocusVisual();
        digitFocus.ResetSignColor();
        state.lastCell = toCellArg;
        wrapFollow = false;
        return false;
    }
    return true;
}

void
DigitFocusDiverge::clearVisual(AnimationGate &gate) {
    if (active) {
        gate.Cancel(AnimationChannel::NonBlocking);
    }
    active = false;
    phase = 0;
    wrapFollow = false;
    phaseKind = Phase::Diverge;
    state.Clear();
    digitFocus.Hide();
    digitFocus.ResetSignColor();
}

bool
DigitFocusDiverge::start(AnimationGate &gate) {
    phase = 0;
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &DigitFocusDiverge::TickThunk,
                              &DigitFocusDiverge::CancelThunk))) {
        active = false;
        return false;
    }
    active = true;
    return true;
}

void
DigitFocusDiverge::Cancel(void) {
    active = false;
    phase = 0;
    wrapFollow = false;
    phaseKind = Phase::Diverge;
    if (digitPad.HasFocus()) {
        digitPad.SnapFocusVisual();
        digitFocus.ResetSignColor();
        state.lastCell = digitPad.FocusCell();
    } else {
        digitFocus.Hide();
        digitFocus.ResetSignColor();
        state.Clear();
    }
}

bool
DigitFocusDiverge::tickDiverge(void) {
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
        LerpRgb15(COLOR_COMMON_BORDER, COLOR_COMMON_BG, eased));

    phase++;
    if (phase < Frames) {
        return true;
    }

    if (!wrapFollow) {
        digitFocus.Hide();
        digitFocus.ResetSignColor();
        active = false;
        return false;
    }

    // Continue in the same NonBlocking slot with converge at the destination.
    phaseKind = Phase::Converge;
    phase = 0;
    setupConvergeCorners(toPx);
    digitFocus.SetSignColor(COLOR_COMMON_BG);
    digitFocus.SetCornerPositions(startCorners);
    digitFocus.Show();
    return true;
}

bool
DigitFocusDiverge::tickConverge(void) {
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
        state.lastCell = toCell;
        wrapFollow = false;
        phaseKind = Phase::Diverge;
        active = false;
        return false;
    }
    return true;
}

bool
DigitFocusDiverge::Tick(void) {
    if (phaseKind == Phase::Converge) {
        return tickConverge();
    }
    return tickDiverge();
}

bool
DigitFocusDiverge::TickThunk(void *ctx) {
    return static_cast<DigitFocusDiverge *>(ctx)->Tick();
}

void
DigitFocusDiverge::CancelThunk(void *ctx) {
    static_cast<DigitFocusDiverge *>(ctx)->Cancel();
}
