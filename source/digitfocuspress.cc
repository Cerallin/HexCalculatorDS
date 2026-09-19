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

DigitFocusPress::DigitFocusPress(EditorView &editorView,
                                 DigitFocusAnimState &state)
    : digitPad(editorView.GetDigitPad()), digitFocus(digitPad.GetDigitFocus()),
      state(state), phase(0), basePx(0, 0) {}

void
DigitFocusPress::AfterHandle(const Event &e, AnimationGate &gate) {
    if (e.type == EventType::InputViewChangedEvent) {
        if (active) {
            gate.Cancel(AnimationChannel::NonBlocking);
        }
        active = false;
        phase = 0;
        state.Clear();
        return;
    }

    if (e.type == EventType::FlipBitEvent) {
        if (!state.HasVisual() || !digitPad.HasFocus()) {
            return;
        }

        if (!Point::SamePosition(state.lastCell, digitPad.FocusCell())) {
            return;
        }

        if (e.data != digitPad.GetFocus()) {
            return;
        }

        basePx = DigitPad::CellToFocusPixel(digitPad.FocusCell());
        digitFocus.SetPixelPosition(basePx);
        digitFocus.Show();
        if (!start(gate)) {
            snapToLogical();
        }
    }
}

bool
DigitFocusPress::start(AnimationGate &gate) {
    phase = 0;
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &DigitFocusPress::TickThunk,
                              &DigitFocusPress::CancelThunk))) {
        active = false;
        return false;
    }
    active = true;
    return true;
}

void
DigitFocusPress::Cancel(void) {
    active = false;
    phase = 0;
    snapToLogical();
}

void
DigitFocusPress::snapToLogical(void) {
    if (digitPad.HasFocus()) {
        digitPad.SnapFocusVisual();
        state.lastCell = digitPad.FocusCell();
    } else {
        digitFocus.Hide();
        state.Clear();
    }
}

bool
DigitFocusPress::Tick(void) {
    const Point pressed(basePx.x, basePx.y + PressOffsetY);

    if (phase < DownFrames) {
        const uint16_t t256 = static_cast<uint16_t>(
            (static_cast<uint32_t>(phase + 1) * 256) / DownFrames);
        digitFocus.SetPixelPosition(basePx.x,
                                    LerpInt(basePx.y, pressed.y, t256));
    } else if (phase < DownFrames + HoldFrames) {
        digitFocus.SetPixelPosition(pressed);
    } else {
        const uint16_t upPhase =
            static_cast<uint16_t>(phase - DownFrames - HoldFrames);
        const uint16_t t256 = static_cast<uint16_t>(
            (static_cast<uint32_t>(upPhase + 1) * 256) / UpFrames);
        digitFocus.SetPixelPosition(basePx.x,
                                    LerpInt(pressed.y, basePx.y, t256));
    }

    phase++;
    if (phase >= TotalFrames) {
        digitFocus.SetPixelPosition(basePx);
        active = false;
        return false;
    }
    return true;
}

bool
DigitFocusPress::TickThunk(void *ctx) {
    return static_cast<DigitFocusPress *>(ctx)->Tick();
}

void
DigitFocusPress::CancelThunk(void *ctx) {
    static_cast<DigitFocusPress *>(ctx)->Cancel();
}
