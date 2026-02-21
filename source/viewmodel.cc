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
      formulaModel(eventBus), valueModel(eventBus), keyInputHandler(commands) {
    eventBus.Subscribe(config);
    eventBus.Subscribe(formulaModel);
    eventBus.Subscribe(valueModel);

    // FIXME just for testing, should be removed later
    commands.InputNumber1();
    commands.InputNumber2();
    commands.InputNumber3();
    commands.InputNumber4();
    commands.InputNumber5();
    commands.InputNumber6();
    commands.InputNumber7();
    commands.InputNumber8();
    commands.InputNumber9();
    commands.InputNumber0();
    commands.InputNumber0();
    commands.InputNumber0();
    commands.InputNumber0();
    commands.InputNumber7();
    commands.InputNumber2();
    commands.InputNumber1();
}

void
ViewModel::DispatchEvents(void) {
    eventBus.DispatchPending();
}

bool
ViewModel::handleKeyInputs(void) {
    auto keyInput = ReadKeyInput();
    if (keyInput.Active()) {
        return keyInputHandler.Handle(keyInput);
    }

    return false;
}

bool
ViewModel::handleTouchScreen(void) {
    auto touchInput = ReadTouchInput();
    if (!touchInput.Active()) {
        return false;
    }

    // Get the touch point and post a TouchScreenEvent with the point data
    Point touchPoint = touchInput.point;
    eventBus.Post(Event{
        touchPoint.ToInt(),
        EventType::TouchScreenEvent,
    });

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
