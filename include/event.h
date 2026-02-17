/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "structure.h"

namespace HexCalc {

enum EventType {
    UnknownEvent = 0,
    // hardware events
    KeysPressedEvent, // when the user presses keys
    TouchScreenEvent, // when the user touches the screen
    // logical events
    InputEvent,       // when the user inputs a number or an operator
    ClearEvent,       // when the user clears the formula
    EvaluateEvent,    // when the user evaluates the formula
    UpdateBaseEvent,  // when the user changes the number base
    UpdateWidthEvent, // when the user changes the number width
    UpdateSignEvent,  // when the user changes the number sign
};

using EventDataType = int;

struct Event {
    EventDataType data;
    EventType type;
};

constexpr size_t EventQueueSize = 16;
extern CircularQueue<Event, EventQueueSize> eventQueue;

static inline bool
PostEvent(Event e) {
    return eventQueue.Enqueue(e);
}

template <typename T>
static inline auto
Dispatch(T &obj, const Event &e) -> decltype(obj.HandleEvent(e)) {
    return obj.HandleEvent(e);
}

enum HandleEventResult {
    Error = -1,    // when the event is invalid or cannot be handled
    Handled = 0,   // when the event is successfully handled
    Ignored = 1,   // when the event is valid but not relevant to the object
    Generated = 2, // when generates new events to be handled
};

}; // namespace HexCalc
