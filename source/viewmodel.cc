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

void
ViewModel::DispatchEvents(void) {
    Event event;
    while (eventQueue.Dequeue(event)) {
        Dispatch(config, event);
        Dispatch(formulaModel, event);
        Dispatch(displayModel, event);
    }
}

void
HexCalc::ViewModel::HandleInputs(void) {
    scanKeys();
    uint32_t keys = keysDownRepeat();

    // TODO generate key events

    // TODO handle touchScreen events

    previousKeys = keys;
}

void
HexCalc::ViewModel::UpdateViews(void) {
    mainView.Update(formulaModel);
    mainView.Update(displayModel);
}
