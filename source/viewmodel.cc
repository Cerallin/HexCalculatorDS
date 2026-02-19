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
    : eventBus(), formulaModel(eventBus), valueModel(eventBus),
      previousKeys(0) {
    eventBus.Subscribe(config);
    eventBus.Subscribe(formulaModel);
    eventBus.Subscribe(valueModel);
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

    eventBus.Post(Event{static_cast<EventDataType>(keyInput.keys),
                        EventType::KeysPressedEvent});
    previousKeys = keyInput.keys;

    return true;
}

bool
ViewModel::handleTouchScreen(void) {
    auto touchInput = ReadTouchInput();
    if (!touchInput.Active()) {
        return false;
    }

    eventBus.Post(
        Event{static_cast<EventDataType>(touchInput.point.ToInt()),
              EventType::TouchScreenEvent});
    return true;
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
