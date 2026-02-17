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
    scanKeys();
    uint32_t keys = keysDownRepeat();

    // TODO generate key events

    previousKeys = keys;

    return keys != 0;
}

bool
ViewModel::handleTouchScreen(void) {
    // TODO handle touchScreen events
    return false;
}

void
ViewModel::HandleInputs(void) {
    if (!handleKeyInputs()) {
        handleTouchScreen();
    }
}

void
ViewModel::UpdateViews(void) {
    // TODO update views
}
