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
readHardwareInput() {
    HardwareInputSnapshot input;

    scanKeys();
    input.heldKeys = keysHeld();
    touchRead(&input.touch);

    return input;
}

void
TouchButton::ExecuteCommand(Commands &commands, ButtonType type, int index) {
    switch (type) {
    case ButtonAnd:
        commands.InputOperator(OperatorType::BitwiseAnd);
        break;
    case ButtonOr:
        commands.InputOperator(OperatorType::BitwiseOr);
        break;
    case ButtonModulo:
        commands.InputOperator(OperatorType::Modulo);
        break;
    case ButtonA:
        commands.InputNumber(DigitA);
        break;
    case ButtonB:
        commands.InputNumber(DigitB);
        break;
    case ButtonC:
        commands.InputNumber(DigitC);
        break;
    case ButtonD:
        commands.InputNumber(DigitD);
        break;
    case ButtonE:
        commands.InputNumber(DigitE);
        break;
    case ButtonF:
        commands.InputNumber(DigitF);
        break;
    case Button0:
        commands.InputNumber(Digit0);
        break;
    case Button1:
        commands.InputNumber(Digit1);
        break;
    case Button2:
        commands.InputNumber(Digit2);
        break;
    case Button3:
        commands.InputNumber(Digit3);
        break;
    case Button4:
        commands.InputNumber(Digit4);
        break;
    case Button5:
        commands.InputNumber(Digit5);
        break;
    case Button6:
        commands.InputNumber(Digit6);
        break;
    case Button7:
        commands.InputNumber(Digit7);
        break;
    case Button8:
        commands.InputNumber(Digit8);
        break;
    case Button9:
        commands.InputNumber(Digit9);
        break;
    case ButtonLShift:
        commands.InputOperator(OperatorType::LeftShift);
        break;
    case ButtonRShift:
        commands.InputOperator(OperatorType::RightShift);
        break;
    case ButtonClear:
        commands.Clear();
        break;
    case ButtonBackspace:
        commands.InputOperatorBackspace();
        break;
    case ButtonLBrac:
        commands.InputOperator(OperatorType::LeftBracket);
        break;
    case ButtonRBrac:
        commands.InputOperator(OperatorType::RightBracket);
        break;
    case ButtonDivide:
        commands.InputOperator(OperatorType::Divide);
        break;
    case ButtonMultiply:
        commands.InputOperator(OperatorType::Multiply);
        break;
    case ButtonMinus:
        commands.InputOperator(OperatorType::Minus);
        break;
    case ButtonPlus:
        commands.InputOperator(OperatorType::Plus);
        break;
    case ButtonNegate:
        commands.InputOperator(OperatorType::Negate);
        break;
    case ButtonBitwiseNot:
        commands.InputOperator(OperatorType::BitwiseNot);
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
    case ButtonLShiftMode:
        commands.SwitchShiftModeLeft();
        break;
    case ButtonRShiftMode:
        commands.SwitchShiftModeRight();
        break;
    case ButtonFlipBit:
        commands.FlipBit(index);
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
    auto input = readHardwareInput();
    bool touched = ((input.heldKeys & KEY_TOUCH) != 0);

    updateKeys(input.heldKeys);
    Point touchPoint(input.touch.px, input.touch.py);
    updateTouch(touched, touchPoint);
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

    for (int i = 0; i < KEY_COUNT; i++) {
        uint32_t mask = BIT(i);

        bool pressed = ((heldKeys & mask) != 0);
        bool prevPressed = ((previousHeldKeys & mask) != 0);
        if (!pressed) {
            if (prevPressed) {
                // key released
                postKeyEvent(KeyAction::PressUp, mask);
            }
            // reset counter for next press
            keyStates[i].counter = 0;
            // skip to next key
            continue;
        }

        auto &s = keyStates[i];

        if (s.counter == 0) {
            // first press
            postKeyEvent(KeyAction::PressDown, mask);
        } else if (s.counter > repeatDelay) {
            assert(repeatRate > 0);
            bool shouldRepeat = (((s.counter - repeatDelay) % repeatRate) == 0);
            if (shouldRepeat) {
                // repeat press
                postKeyEvent(KeyAction::PressDown, mask);
            }
        }

        // increment counter for next frame
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

    // post events
    if (!previousTouch && stablePressed) {
        postTouchEvent(TouchAction::TouchDown, smoothPos);
    } else if (previousTouch && !stablePressed) {
        postTouchEvent(TouchAction::TouchUp, smoothPos);
    } else {
        // no change, do nothing
    }
}

void
InputHandler::postKeyEvent(KeyAction action, uint32_t keyMask) {
    EventType actionType =
        (action == KeyAction::PressDown) ? EventType::KeyPressDownEvent
        : (action == KeyAction::PressUp) ? EventType::KeyPressUpEvent
                                         : EventType::UnknownEvent;

    eventBus.Post(Event{
        .data = static_cast<EventDataType>(keyMask),
        .type = actionType,
    });
}

void
InputHandler::postTouchEvent(TouchAction action, const Point &pos) {
    EventType actionType =
        (action == TouchAction::TouchDown) ? EventType::TouchDownEvent
        : (action == TouchAction::TouchUp) ? EventType::TouchUpEvent
                                           : EventType::UnknownEvent;

    eventBus.Post(Event{
        .data = pos.ToInt(),
        .type = actionType,
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
        commands.InputPrevious();
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
