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

ViewModel::ViewModel(void)
    : // displays
      mainDisplay(), subDisplay(),
      // models
      formulaModel(), valueModel(),
      // main screen views
      formulaView(Area(3, 0, 30, 2), ViewAlign::AlignRight, mainDisplay),
      valueView(Area(5, 0, 30, 3), ViewAlign::AlignRight, mainDisplay),
      // There's a gap between valueView and hexView
      // TODO use an empty view for the gap
      hexView(Area(7, 0, 30, 2), ViewAlign::AlignLeft, mainDisplay),
      decView(Area(9, 0, 30, 2), ViewAlign::AlignLeft, mainDisplay),
      octView(Area(11, 0, 30, 3), ViewAlign::AlignLeft, mainDisplay),
      // There's a gap between decView and octView
      binView(Area(13, 0, 30, 8), ViewAlign::AlignLeft, mainDisplay),
      // subscreen view
      inputView(subDisplay),
      // input state
      previousKeys(0)
//
{
    //
}

void
ViewModel::DispatchEvents(void) {
    Event event;
    while (eventQueue.Dequeue(event)) {
        Dispatch(config, event);
        Dispatch(formulaModel, event);
        Dispatch(valueModel, event);
        Dispatch(formulaView, event);
        Dispatch(valueView, event);
        Dispatch(hexView, event);
        Dispatch(decView, event);
        Dispatch(octView, event);
        Dispatch(binView, event);
        Dispatch(inputView, event);
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
