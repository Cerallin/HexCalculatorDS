/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "input.h"
#include "commands.h"

#include <nds.h>

using namespace HexCalc;

struct HardwareInputSnapshot {
    uint32_t heldKeys = 0;
    touchPosition touch{};
};

static HardwareInputSnapshot
ReadHardwareInput() {
    scanKeys();

    HardwareInputSnapshot input;
    input.heldKeys = keysHeld();
    touchRead(&input.touch);
    return input;
}

void
TouchButton::ExecuteCommand(Commands &commands, ButtonType type) {
    switch (type) {
    case ButtonAnd:
        commands.InputOperatorAnd();
        break;
    case ButtonOr:
        commands.InputOperatorOr();
        break;
    case ButtonModulo:
        commands.InputOperatorModulo();
        break;
    case ButtonA:
        commands.InputNumberA();
        break;
    case ButtonLShift:
        commands.InputOperatorLShift();
        break;
    case ButtonRShift:
        commands.InputOperatorRShift();
        break;
    case ButtonClear:
        commands.Clear();
        break;
    case ButtonBackspace:
        commands.InputOperatorBackspace();
        break;
    case ButtonB:
        commands.InputNumberB();
        break;
    case ButtonLBrac:
        commands.InputOperatorLBrac();
        break;
    case ButtonRBrac:
        commands.InputOperatorRBrac();
        break;
    case ButtonDivide:
        commands.InputOperatorDivide();
        break;
    case ButtonMultiply:
        commands.InputOperatorMultiply();
        break;
    case ButtonC:
        commands.InputNumberC();
        break;
    case Button7:
        commands.InputNumber7();
        break;
    case Button8:
        commands.InputNumber8();
        break;
    case Button9:
        commands.InputNumber9();
        break;
    case ButtonMinus:
        commands.InputOperatorMinus();
        break;
    case ButtonD:
        commands.InputNumberD();
        break;
    case Button4:
        commands.InputNumber4();
        break;
    case Button5:
        commands.InputNumber5();
        break;
    case Button6:
        commands.InputNumber6();
        break;
    case ButtonPlus:
        commands.InputOperatorPlus();
        break;
    case ButtonE:
        commands.InputNumberE();
        break;
    case Button1:
        commands.InputNumber1();
        break;
    case Button2:
        commands.InputNumber2();
        break;
    case Button3:
        commands.InputNumber3();
        break;
    case ButtonF:
        commands.InputNumberF();
        break;
    case ButtonNegate:
        commands.InputOperatorNegate();
        break;
    case Button0:
        commands.InputNumber0();
        break;
    case ButtonBitwiseNot:
        commands.InputOperatorNot();
        break;
    case ButtonEvaluate:
        commands.Evaluate();
        break;
    case ButtonChangeWidth:
        commands.SwitchWidthLower();
        break;
    case ButtonToggleSign:
        commands.ToggleSign();
        break;
    default:
        break;
    }
}

InputHandler::InputHandler(EventBus &eventBus, Commands &commands)
    : eventBus(eventBus), commands(commands) {
    eventBus.Subscribe(*this);
}

void
InputHandler::Update(void) {
    auto input = ReadHardwareInput();
    bool touched = (input.heldKeys & KEY_TOUCH) != 0;

    updateKeys(input.heldKeys);
    updateTouch(touched, Point(input.touch.px, input.touch.py));
}

void
InputHandler::SetRepeat(int delay, int rate) {
    repeatDelay = delay;
    repeatRate = rate;
}

void
InputHandler::updateKeys(uint32_t newHeldKeys) {
    previousHeldKeys = heldKeys;
    heldKeys = newHeldKeys;

    for (int i = 0; i < 32; ++i) {
        uint32_t mask = (1u << i);

        if (!(heldKeys & mask)) {
            if (previousHeldKeys & mask) {
                postKeyEvent(KeyAction::PressUp, mask);
            }
            keyStates[i].counter = 0;
            continue;
        }

        auto &s = keyStates[i];

        if (s.counter == 0) {
            postKeyEvent(KeyAction::PressDown, mask);
        } else if (s.counter > repeatDelay) {
            if (repeatRate > 0 &&
                ((s.counter - repeatDelay) % repeatRate == 0)) {
                postKeyEvent(KeyAction::PressDown, mask);
            }
        }

        s.counter++;
    }
}

void
InputHandler::updateTouch(bool rawPressed, const Point &rawPoint) {
    previousTouch = stablePressed;

    // debounce
    if (rawPressed) {
        pressCount++;
        releaseCount = 0;
        if (pressCount >= PRESS_TH) {
            stablePressed = true;
        }
    } else {
        pressCount = 0;
        releaseCount++;
        if (releaseCount >= RELEASE_TH) {
            stablePressed = false;
        }
    }

    // update position
    if (stablePressed) {
        if (!previousTouch) {
            // use raw position for the first frame to avoid lag
            smoothPos = rawPoint;
        } else {
            // smooth the position to reduce jitter
            smoothPos.x = (smoothPos.x + rawPoint.x) / 2;
            smoothPos.y = (smoothPos.y + rawPoint.y) / 2;
        }
    }

    if (!previousTouch && stablePressed) {
        postTouchEvent(TouchAction::TouchDown, smoothPos);
    } else if (previousTouch && !stablePressed) {
        postTouchEvent(TouchAction::TouchUp, smoothPos);
    }
}

void
InputHandler::postKeyEvent(KeyAction action, uint32_t keyMask) {
    eventBus.Post(Event{
        .data = static_cast<EventDataType>(keyMask),
        .type = action == KeyAction::PressDown ? EventType::KeyPressDownEvent
                                               : EventType::KeyPressUpEvent,
    });
}

void
InputHandler::postTouchEvent(TouchAction action, const Point &pos) {
    eventBus.Post(Event{
        .data = pos.ToInt(),
        .type = action == TouchAction::TouchDown ? EventType::TouchDownEvent
                                                 : EventType::TouchUpEvent,
    });
}

void
InputHandler::dispatchKeyPressDown(uint32_t keyMask) {
    if (keyMask & KEY_UP) { // ↑
        commands.MoveFocusUp();
    } else if (keyMask & KEY_DOWN) { // ↓
        commands.MoveFocusDown();
    } else if (keyMask & KEY_LEFT) { // ←
        commands.MoveFocusLeft();
    } else if (keyMask & KEY_RIGHT) { // →
        commands.MoveFocusRight();
    } else if (keyMask & KEY_A) { // A
        eventBus.Post(Event{.data = 0, .type = PreviousTouchEvent});
    } else if (keyMask & KEY_B) { // B
        commands.InputOperatorBackspace();
    } else if (keyMask & KEY_X) { // X
        commands.Clear();
    } else if (keyMask & KEY_Y) { // Y
        commands.SwitchWidthUpper();
    } else if (keyMask & KEY_SELECT) { // Select
        commands.SwitchBaseLower();
    } else if (keyMask & KEY_START) { // Start
        commands.Evaluate();
    } else if (keyMask & KEY_L) { // L
        commands.SwitchFormulaPageLeft();
    } else if (keyMask & KEY_R) { // R
        commands.SwitchFormulaPageRight();
    }
}

EventResult
InputHandler::HandleEvent(const Event &e) {
    if (e.type == KeyPressDownEvent) {
        dispatchKeyPressDown(static_cast<uint32_t>(e.data));
        return Consumed;
    }

    if (e.type == TouchDownEvent) {
        eventBus.Post(Event{
            .data = e.data,
            .type = TouchScreenEvent,
        });
        return Emitted;
    }

    if (e.type == TouchUpEvent) {
        return Consumed;
    }

    return Skipped;
}
