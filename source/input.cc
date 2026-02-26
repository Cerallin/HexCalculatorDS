/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "input.h"
#include "commands.h"

using namespace HexCalc;

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

void
InputHandler::Update(void) {
    scanKeys();

    updateKeys();
    updateTouch();

    handleKeyInput(KeyInput{currentKeys});
    handleTouchInput(TouchInput{smoothPos, stablePressed});
}

bool
InputHandler::KeyDown(uint32_t k) const {
    return (currentKeys & k) && !(previousKeys & k);
}

bool
InputHandler::KeyUp(uint32_t k) const {
    return !(currentKeys & k) && (previousKeys & k);
}

bool
InputHandler::TouchDown() const {
    return stablePressed && !previousTouch;
}

bool
InputHandler::TouchUp() const {
    return !stablePressed && previousTouch;
}

void
InputHandler::updateKeys(void) {
    previousKeys = currentKeys;
    currentKeys = keysHeld();
}

void
InputHandler::updateTouch() {
    previousTouch = stablePressed;

    touchPosition pos{};
    touchRead(&pos);

    bool rawPressed = currentKeys & KEY_TOUCH;

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
            smoothPos.x = pos.px;
            smoothPos.y = pos.py;
        } else {
            // smooth the position to reduce jitter
            smoothPos.x = (smoothPos.x + pos.px) / 2;
            smoothPos.y = (smoothPos.y + pos.py) / 2;
        }
    }
}

void
InputHandler::handleKeyInput(const KeyInput &input) {
    if (input.PressedUp()) {
        commands.SwitchBaseUpper();
    } else if (input.PressedDown()) {
        commands.SwitchBaseLower();
    } else if (input.PressedLeft()) {
        // do nothing for now
    } else if (input.PressedRight()) {
        // do nothing for now
    } else if (input.PressedA()) {
        // TODO input previous selected button
    } else if (input.PressedB()) {
        commands.InputOperatorBackspace();
    } else if (input.PressedX()) {
        commands.Clear();
    } else if (input.PressedY()) {
        // TODO switch width
    } else if (input.PressedStart()) {
        commands.Evaluate();
    } else if (input.PressedL()) {
        // TODO
    } else if (input.PressedR()) {
        // TODO
    }
}

void
InputHandler::handleTouchInput(const TouchInput &input) {
    if (TouchDown()) {
        eventBus.Post(Event{
            .data = input.point.ToInt(),
            .type = TouchScreenEvent,
        });
    }
}
