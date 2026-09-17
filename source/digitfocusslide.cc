/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationeffects.h"
#include "viewhost.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

namespace {

constexpr int
sign(int delta) {
    if (delta > 0) {
        return 1;
    }
    if (delta < 0) {
        return -1;
    }
    return 0;
}

} // namespace

DigitFocusSlide::DigitFocusSlide(EditorView &editorView,
                                 DigitFocusAnimState &state,
                                 DigitFocusDiverge &diverge)
    : digitPad(editorView.GetDigitPad()), digitFocus(digitPad.GetDigitFocus()),
      state(state), diverge(diverge), phase(0), fromPx(0, 0), toPx(0, 0) {}

void
DigitFocusSlide::AfterHandle(const Event &e, AnimationGate &gate) {
    if (e.type == EventType::InputViewChangedEvent) {
        if (active) {
            gate.Cancel(AnimationChannel::NonBlocking);
        }
        active = false;
        phase = 0;
        state.Clear();
        return;
    }

    if ((e.type != EventType::MoveFocusEvent) &&
        (e.type != EventType::FlipBitEvent)) {
        return;
    }

    if (!digitPad.HasFocus() || !state.HasVisual()) {
        return;
    }

    const Point cell = digitPad.FocusCell();
    if (Point::SamePosition(cell, state.lastCell)) {
        return;
    }

    if ((e.type == EventType::MoveFocusEvent) && digitPad.LastFocusWrapped()) {
        const Point fromCell = state.lastCell;
        diverge.StartWrap(fromCell, cell, gate);
        return;
    }

    // Prefer last cell pixel over GetPixelPosition: DigitPad already snapped to
    // the new cell before AfterHandle runs.
    fromPx = DigitPad::CellToFocusPixel(state.lastCell);
    toPx = DigitPad::CellToFocusPixel(cell);
    if (Point::SamePosition(fromPx, toPx)) {
        state.lastCell = cell;
        return;
    }

    digitFocus.SetPixelPosition(fromPx);
    digitFocus.Show();
    if (!start(gate)) {
        snapToLogical();
        return;
    }
    state.lastCell = cell;
}

bool
DigitFocusSlide::start(AnimationGate &gate) {
    phase = 0;
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &DigitFocusSlide::TickThunk,
                              &DigitFocusSlide::CancelThunk))) {
        active = false;
        return false;
    }
    active = true;
    return true;
}

void
DigitFocusSlide::Cancel(void) {
    active = false;
    phase = 0;
}

void
DigitFocusSlide::snapToLogical(void) {
    if (digitPad.HasFocus()) {
        digitPad.SnapFocusVisual();
        state.lastCell = digitPad.FocusCell();
    } else {
        digitFocus.Hide();
        state.Clear();
    }
}

bool
DigitFocusSlide::Tick(void) {
    if (!active) {
        return false;
    }

    const int sx = sign(toPx.x - fromPx.x);
    const int sy = sign(toPx.y - fromPx.y);
    const Point overshoot(toPx.x + sx, toPx.y + sy);

    if (phase < MoveFrames) {
        const uint16_t t256 = static_cast<uint16_t>(
            (static_cast<uint32_t>(phase + 1) * 256) / MoveFrames);
        const uint16_t eased = EaseInOutCubicBezier(t256);
        int16_t x = LerpInt(fromPx.x, overshoot.x, eased);
        int16_t y = LerpInt(fromPx.y, overshoot.y, eased);
        digitFocus.SetPixelPosition(x, y);
    } else {
        const uint16_t settlePhase = static_cast<uint16_t>(phase - MoveFrames);
        const uint16_t t256 = static_cast<uint16_t>(
            (static_cast<uint32_t>(settlePhase + 1) * 256) / SettleFrames);
        int16_t x = LerpInt(overshoot.x, toPx.x, t256);
        int16_t y = LerpInt(overshoot.y, toPx.y, t256);
        digitFocus.SetPixelPosition(x, y);
    }

    phase++;
    if (phase >= TotalFrames) {
        digitFocus.SetPixelPosition(toPx);
        active = false;
        return false;
    }
    return true;
}

bool
DigitFocusSlide::TickThunk(void *ctx) {
    return static_cast<DigitFocusSlide *>(ctx)->Tick();
}

void
DigitFocusSlide::CancelThunk(void *ctx) {
    static_cast<DigitFocusSlide *>(ctx)->Cancel();
}
