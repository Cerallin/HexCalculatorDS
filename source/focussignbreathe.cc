/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "focussignbreathe.h"
#include "theme.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

FocusSignBreathe::FocusSignBreathe(InputView &inputView, SubDisplay &display)
    : inputView(inputView), display(display),
      curve(COLOR_COMMON_BORDER, COLOR_COMMON_BG, Period, BezierTriangle),
      active(false), buttonIndex(-1), phase(0) {}

bool
FocusSignBreathe::TryHandle(const Event &, AnimationGate &) {
    return false;
}

bool
FocusSignBreathe::Suppress(const Event &) const {
    return false;
}

bool
FocusSignBreathe::IsActive(void) const {
    return active;
}

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

bool
FocusSignBreathe::SuppressesViewUpdate(void) const {
    return false;
}

void
FocusSignBreathe::sync(AnimationGate &gate) {
    // Get the focused keyboard index
    const int focused = inputView.FocusedKeyboardIndex();
    if (focused < 0) { // No focused keyboard index, cancel the animation.
        gate.Cancel(AnimationChannel::NonBlocking);
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
    // If the animation is not active or the button index is invalid, do
    // nothing
    if (!active || buttonIndex < 0) {
        return false;
    }

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
