/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationeffects.h"
#include "theme.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

FocusSignBreathe::FocusSignBreathe(InputView &inputView, SubDisplay &display)
    : inputView(inputView), display(display),
      curve(COLOR_COMMON_BORDER, COLOR_COMMON_BG, Period, BezierTriangle),
      buttonIndex(-1), phase(0) {}

void
FocusSignBreathe::AfterHandle(const Event &e, AnimationGate &gate) {
    // Sync when focus may change or the focused button may become inactive.
    switch (e.type) {
    case EventType::MoveFocusEvent:
        sync(gate);
        break;
    case EventType::TouchScreenEvent:
        sync(gate);
        break;
    case EventType::UpdateBaseEvent:
        sync(gate);
        break;
    case EventType::ClearEvent:
        sync(gate);
        break;
    default:
        break;
    }
}

void
FocusSignBreathe::Cancel(void) {
    if (!active) {
        return;
    }

    const int previous = buttonIndex;
    active = false;
    buttonIndex = -1;
    phase = 0;

    if (previous >= 0) {
        if (inputView.IsKeyboardButtonActive(previous)) {
            display.EnableButton(previous);
        } else {
            display.DisableButton(previous);
        }
    }
}

void
FocusSignBreathe::sync(AnimationGate &gate) {
    // Get the focused keyboard index
    const int focused = inputView.FocusedKeyboardIndex();
    if (focused < 0) {
        // Only cancel when this effect owns NonBlocking; otherwise ClearEvent /
        // TouchScreenEvent in EditorView would kill DigitFocusDiverge
        // mid-start.
        if (active) {
            gate.Cancel(AnimationChannel::NonBlocking);
        }
        return;
    }

    // Already animating the same button, do nothing
    if (active && focused == buttonIndex) {
        return;
    }

    // Start the animation.
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &FocusSignBreathe::TickThunk,
                              &FocusSignBreathe::CancelThunk))) {
        active = false;
        buttonIndex = -1;
        phase = 0;
        return;
    }

    // Update the button index and phase.
    buttonIndex = focused;
    phase = 0;
    active = true;
}

bool
FocusSignBreathe::Tick(void) {
    display.SetButtonSignColor(buttonIndex, curve.At(phase));
    phase = static_cast<uint16_t>((phase + 1) % curve.Period());
    return true;
}

bool
FocusSignBreathe::TickThunk(void *ctx) {
    return static_cast<FocusSignBreathe *>(ctx)->Tick();
}

void
FocusSignBreathe::CancelThunk(void *ctx) {
    static_cast<FocusSignBreathe *>(ctx)->Cancel();
}
