/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewmodel.h"
#include "config.h"
#include "event.h"
#include "input.h"

using namespace HexCalc;

ViewModel::ViewModel(void)
    : eventBus(),
      // commands
      commands(eventBus),
      // models
      formulaModel(eventBus), valueModel(eventBus) {
    eventBus.Subscribe(config);
    eventBus.Subscribe(formulaModel);
    eventBus.Subscribe(valueModel);

    eventBus.Post(Event{0, EventType::ClearEvent});
}

void
ViewModel::DispatchEvents(void) {
    eventBus.DispatchPending();
}

bool
ViewModel::handleKeyInputs(void) {
    auto keyInput = ReadKeyInput();
    if (!keyInput.Active()) {
        return false;
    }

    if (keyInput.PressedUp()) {
        commands.SwitchBaseUpper();
    } else if (keyInput.PressedDown()) {
        commands.SwitchBaseLower();
    } else if (keyInput.PressedLeft()) {
        // do nothing for now
    } else if (keyInput.PressedRight()) {
        // do nothing for now
    } else if (keyInput.PressedA()) {
        // TODO input previous selected button
    } else if (keyInput.PressedB()) {
        commands.InputOperatorBackspace();
    } else if (keyInput.PressedX()) {
        commands.InputOperatorClear();
    } else if (keyInput.PressedY()) {
        // TODO switch width
    } else if (keyInput.PressedStart()) {
        commands.InputOperatorEqual();
    } else if (keyInput.PressedL()) {
        // TODO
    } else if (keyInput.PressedR()) {
        // TODO
    }

    previousKeys = keyInput.keys;

    return true;
}

bool
ViewModel::handleTouchScreen(void) {
    auto touchInput = ReadTouchInput();
    if (!touchInput.Active()) {
        return false;
    }
    // TODO: handle touch input and post TouchScreenEvent
    return false;
}

void
ViewModel::HandleInputs(void) {
    // First try to handle key inputs,
    // if no keys are pressed, try to handle touch screen inputs.
    if (!handleKeyInputs()) {
        handleTouchScreen();
    }
}

EventBus &
ViewModel::Bus(void) {
    return eventBus;
}

FormulaModel &
ViewModel::Formula(void) {
    return formulaModel;
}

const FormulaModel &
ViewModel::Formula(void) const {
    return formulaModel;
}

ValueModel &
ViewModel::Value(void) {
    return valueModel;
}

const ValueModel &
ViewModel::Value(void) const {
    return valueModel;
}
