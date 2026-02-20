/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "structure.h"

namespace HexCalc {

/**
 * @brief Enumeration of event types.
 *
 */
enum EventType {
    UnknownEvent = 0,
    // logical events
    InputEvent,          // when the user inputs a number or an operator
    ClearEvent,          // when the user clears the formula
    EvaluateEvent,       // when the user evaluates the formula
    UpdateBaseEvent,     // when the user changes the number base
    UpdateWidthEvent,    // when the user changes the number width
    UpdateSignEvent,     // when the user changes the number sign
    FormulaChangedEvent, // formula model invalidated
    ValueChangedEvent,   // value model invalidated
};

using EventDataType = int32_t;

struct Event {
    EventDataType data;
    EventType type;
};

/**
 * @brief Enumeration of event handling results.
 *
 */
enum EventResult {
    Failed = -1,  // invalid or processing failed
    Consumed = 0, // event handled and consumed
    Skipped = 1,  // not relevant to this handler
    Emitted = 2   // handled and emitted new events
};

/**
 * @brief Event listener that can handle events of a specific type. It contains
 * a handler function that processes the event and returns an EventResult
 * indicating the outcome of the event handling.
 *
 */
class EventListener {
  public:
    using Handler = EventResult (*)(void *, const Event &);

    EventListener() : context(nullptr), handler(nullptr) {}

    template <typename T>
    static EventListener
    From(T &instance) {
        return EventListener(static_cast<void *>(&instance),
                             [](void *ctx, const Event &event) -> EventResult {
                                 return static_cast<T *>(ctx)->HandleEvent(
                                     event);
                             });
    }

    EventResult
    OnEvent(const Event &event) const {
        if (handler == nullptr) {
            return Skipped;
        }
        return handler(context, event);
    }

    bool
    Valid(void) const {
        return handler != nullptr;
    }

  private:
    EventListener(void *context, Handler handler)
        : context(context), handler(handler) {}

    void *context;
    Handler handler;
};

/**
 * @brief Event dispatcher that manages event listeners and dispatches events to
 * them.
 *
 * @tparam N The maximum number of event listeners that can be subscribed to the
 * dispatcher.
 */
template <size_t ListenerCapacity, size_t QueueCapacity>
class EventDispatcher : NonCopyable {
  public:
    EventDispatcher() : eventQueue(), listeners(), listenerCount(0) {}

    /**
     * @brief Publish an event to all subscribed listeners.
     *
     * @param e The event to be published.
     */
    void
    Publish(const Event &e) {
        for (size_t i = 0; i < listenerCount; ++i) {
            listeners[i].OnEvent(e);
        }
    }

    /**
     * @brief Subscribe an event listener to the dispatcher.
     *
     * @param listener The event listener to be subscribed.
     * @return true if the listener was successfully subscribed, false if the
     * maximum number of listeners has been reached.
     */
    template <typename Listener>
    bool
    Subscribe(Listener &listener) {
        if (listenerCount >= ListenerCapacity) {
            return false; // no more space for listeners
        }
        listeners[listenerCount++] = EventListener::From(listener);
        return true;
    }

    bool
    Post(const Event &event) {
        return eventQueue.Enqueue(event);
    }

    void
    DispatchPending(void) {
        Event pending;
        while (eventQueue.Dequeue(pending)) {
            Publish(pending);
        }
    }

  private:
    CircularQueue<Event, QueueCapacity> eventQueue;
    EventListener listeners[ListenerCapacity];
    size_t listenerCount;
};

static constexpr size_t DefaultEventListenerCapacity = 16;
static constexpr size_t DefaultEventQueueCapacity = 32;

using EventBus =
    EventDispatcher<DefaultEventListenerCapacity, DefaultEventQueueCapacity>;

}; // namespace HexCalc
