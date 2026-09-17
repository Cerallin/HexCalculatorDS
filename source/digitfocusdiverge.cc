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

int
lerpInt(int from, int to, uint16_t t256) {
    if (t256 >= 256) {
        return to;
    }
    const int delta = to - from;
    return from + (delta * static_cast<int>(t256)) / 256;
}

} // namespace

DigitFocusDiverge::DigitFocusDiverge(EditorView &editorView,
                                     DigitFocusAnimState &state)
    : digitPad(editorView.GetDigitPad()), digitFocus(digitPad.GetDigitFocus()),
      state(state), phase(0), startCorners{Point(0, 0), Point(0, 0),
                                           Point(0, 0), Point(0, 0)},
      endCorners{Point(0, 0), Point(0, 0), Point(0, 0), Point(0, 0)} {}

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
        const Point base = digitFocus.GetPixelPosition();
        Point locked[DigitFocus::CornerCount] = {Point(0, 0), Point(1, 0),
                                                 Point(0, 7), Point(1, 7)};
        DigitFocus::LockedCornerOffsets(locked);
        for (int i = 0; i < DigitFocus::CornerCount; i++) {
            int16_t x = base.x + locked[i].x;
            int16_t y = base.y + locked[i].y;
            int16_t outwardX = kOutward[i].x * SpreadPx;
            int16_t outwardY = kOutward[i].y * SpreadPx;

            startCorners[i].x = x;
            startCorners[i].y = y;
            endCorners[i].x = x + outwardX;
            endCorners[i].y = y + outwardY;
        }

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

void
DigitFocusDiverge::clearVisual(AnimationGate &gate) {
    if (active) {
        gate.Cancel(AnimationChannel::NonBlocking);
    }
    active = false;
    phase = 0;
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
    if (!digitPad.HasFocus()) {
        digitFocus.Hide();
    }
    digitFocus.ResetSignColor();
}

bool
DigitFocusDiverge::Tick(void) {
    if (!active) {
        return false;
    }

    const uint16_t t256 = static_cast<uint16_t>(
        (static_cast<uint32_t>(phase + 1) * 256) / Frames);
    const uint16_t eased = EaseInOutCubicBezier(t256);

    Point corners[DigitFocus::CornerCount] = {Point(0, 0), Point(0, 0),
                                              Point(0, 0), Point(0, 0)};
    for (int i = 0; i < DigitFocus::CornerCount; i++) {
        corners[i].x = lerpInt(startCorners[i].x, endCorners[i].x, eased);
        corners[i].y = lerpInt(startCorners[i].y, endCorners[i].y, eased);
    }
    digitFocus.SetCornerPositions(corners);
    digitFocus.SetSignColor(
        LerpRgb15(COLOR_COMMON_BORDER, COLOR_COMMON_BG, eased));

    phase++;
    if (phase >= Frames) {
        digitFocus.Hide();
        digitFocus.ResetSignColor();
        active = false;
        return false;
    }
    return true;
}

bool
DigitFocusDiverge::TickThunk(void *ctx) {
    return static_cast<DigitFocusDiverge *>(ctx)->Tick();
}

void
DigitFocusDiverge::CancelThunk(void *ctx) {
    static_cast<DigitFocusDiverge *>(ctx)->Cancel();
}
