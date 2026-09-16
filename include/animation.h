/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "structure.h"

namespace HexCalc {

constexpr size_t MaxSlots = 4;
constexpr size_t MaxEffects = 4;

/**
 * @brief One-shot animation slot driven each frame by AnimationGate.
 */
class Animation {
  public:
    using TickFn = bool (*)(void *);

    Animation(void *ctx, TickFn tick) : ctx(ctx), tick(tick) {}
    Animation(void) : Animation(nullptr, nullptr) {}

    bool
    Valid(void) const {
        return tick != nullptr;
    }

    bool
    Tick(void) const {
        return tick(ctx);
    }

  private:
    void *ctx;
    TickFn tick;
};

/**
 * @brief Fixed-capacity LIFO animation runner. Busy() gates input in the main
 * loop. Newest animation is ticked first until it finishes.
 */
template <size_t MaxSlots>
class TAnimationGate : private NonCopyable {
  public:
    TAnimationGate(void) : slots() {}

    bool
    Start(const Animation &a) {
        return slots.Push(a);
    }

    void
    Update(void) {
        Animation a;
        if (!slots.Pop(a)) {
            return;
        }
        if (a.Tick()) {
            slots.Push(a);
        }
    }

    bool
    Busy(void) const {
        return !slots.Empty();
    }

  private:
    Stack<Animation, MaxSlots> slots;
};

using AnimationGate = TAnimationGate<MaxSlots>;

/**
 * @brief Type-erased animation effect plugged into Animated.
 */
class AnimEffect {
  public:
    using TryHandleFn = bool (*)(void *ctx, const Event &e,
                                 AnimationGate &gate);
    using SuppressFn = bool (*)(void *ctx, const Event &e);
    using IsActiveFn = bool (*)(void *ctx);

    AnimEffect(void *ctx, TryHandleFn tryHandle, SuppressFn suppress,
               IsActiveFn isActive)
        : ctx(ctx), tryHandle(tryHandle), suppress(suppress),
          isActive(isActive) {}

    AnimEffect(void) : AnimEffect(nullptr, nullptr, nullptr, nullptr) {}

    template <typename T>
    static AnimEffect
    From(T &effect) {
        return AnimEffect(
            &effect,
            [](void *ctx, const Event &e, AnimationGate &gate) -> bool {
                return static_cast<T *>(ctx)->TryHandle(e, gate);
            },
            [](void *ctx, const Event &e) -> bool {
                return static_cast<T *>(ctx)->Suppress(e);
            },
            [](void *ctx) -> bool {
                return static_cast<T *>(ctx)->IsActive();
            });
    }

    bool
    TryHandle(const Event &e, AnimationGate &gate) const {
        return tryHandle(ctx, e, gate);
    }

    bool
    Suppress(const Event &e) const {
        return suppress(ctx, e);
    }

    bool
    IsActive(void) const {
        return isActive(ctx);
    }

  private:
    void *ctx;
    TryHandleFn tryHandle;
    SuppressFn suppress;
    IsActiveFn isActive;
};

/**
 * @brief Per-view proxy: single Subscribe/Update entry, multiple Effect slots.
 */
template <typename View, size_t MaxEffects>
class TAnimated {
  public:
    TAnimated(View &view, AnimationGate &gate)
        : view(view), gate(gate), effectCount(0) {}

    bool
    Add(const AnimEffect &effect) {
        if (effectCount >= MaxEffects) {
            return false;
        }

        effects[effectCount++] = effect;

        return true;
    }

    template <typename Effect>
    bool
    Add(Effect &effect) {
        return Add(AnimEffect::From(effect));
    }

    /**
     * @brief Bypass view.HandleEvent()
     *
     * @param e The event to handle.
     * @return EventResult The result of the event handling.
     */
    EventResult
    HandleEvent(const Event &e) {
        for (size_t i = 0; i < effectCount; i++) {
            if (effects[i].Suppress(e)) {
                return Consumed;
            }
        }
        for (size_t i = 0; i < effectCount; i++) {
            if (effects[i].TryHandle(e, gate)) {
                return Consumed;
            }
        }

        return view.HandleEvent(e);
    }

    /**
     * @brief Bypass view.Update()
     *
     */
    void
    Update(void) {
        if (gate.Busy()) {
            // do nothing
        } else {
            view.Update();
        }
    }

  private:
    View &view;
    AnimationGate &gate;
    AnimEffect effects[MaxEffects];
    size_t effectCount;
};

template <typename View>
using Animated = TAnimated<View, MaxEffects>;

}; // namespace HexCalc
