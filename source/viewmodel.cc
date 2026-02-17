/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewmodel.h"
#include "config.h"
#include "event.h"

using namespace HexCalc;

CircularQueue<Event, EventQueueSize> HexCalc::eventQueue;

void
ViewModel::DispatchEvents(void) {
    Event event;
    while (eventQueue.Dequeue(event)) {
        Dispatch(config, event);
        Dispatch(formulaModel, event);
        Dispatch(valueModel, event);
    }
}

bool
ViewModel::handleKeyInputs(void) {
    // TODO adjust keysSetRepeat()
    scanKeys();
    uint32_t keys = keysDownRepeat();
    // Post an event if there are any keys pressed, otherwise do nothing.
    if (keys == 0) {
        return false;
    }

    PostEvent(Event{EventDataType(keys), EventType::KeysPressedEvent});
    previousKeys = keys;

    return keys != 0;
}

bool
ViewModel::handleTouchScreen(void) {
    touchPosition touchPosition;
    touchRead(&touchPosition);
    Point touchPoint(touchPosition);
    // Post an event if there is a touch, otherwise do nothing.
    if (touchPosition.px == 0 && touchPosition.py == 0) {
        return false;
    }
    PostEvent(Event{touchPoint.ToInt(), EventType::TouchScreenEvent});
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

void
ViewModel::UpdateViews(void) {
    // TODO update views
}
