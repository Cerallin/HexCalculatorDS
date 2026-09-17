/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "traits.h"

namespace HexCalc {

constexpr size_t MaxEffects = 8;

/**
 * @brief Animation slots keyed by input-blocking policy, not by effect kind.
 *        Channels are ticked in parallel each frame.
 */
enum class AnimationChannel : uint8_t {
    Blocking = 0,    // blocks input; cannot Cancel/Replace
    NonBlocking = 1, // does not block input; Start replaces after Cancel
    Count
};

/**
 * @brief One animation instance driven each frame by AnimationGate.
 */
class Animation {
  public:
    using TickFn = bool (*)(void *);
    using CancelFn = void (*)(void *);

    Animation(void *ctx, TickFn tick, CancelFn cancel = nullptr)
        : ctx(ctx), tick(tick), cancel(cancel) {}
    Animation(void) : Animation(nullptr, nullptr, nullptr) {}

    bool
    Valid(void) const {
        return tick != nullptr;
    }

    bool
    Tick(void) const {
        return tick(ctx);
    }

    void
    Cancel(void) const {
        cancel(ctx);
    }

  private:
    void *ctx;
    TickFn tick;
    CancelFn cancel;
};

/**
 * @brief Per-channel animation runner. Busy() is true only while Blocking is
 *        occupied (main-loop input gate).
 */
class AnimationGate : private NonCopyable {
  public:
    AnimationGate(void) : slots() {}

    static constexpr size_t ChannelCount =
        static_cast<size_t>(AnimationChannel::Count);

    /**
     * @brief Start an animation on a given channel.
     *
     * @param channel The channel to start the animation on.
     * @param animation The animation to start.
     * @return true if the animation was started successfully, false otherwise.
     */
    bool
    Start(AnimationChannel channel, const Animation &animation) {
        const size_t index = static_cast<size_t>(channel);
        if (index >= ChannelCount) {
            return false;
        }

        Animation &slot = slots[index];
        if (slot.Valid()) {
            // If the channel is blocking, do not start the animation.
            if (channel == AnimationChannel::Blocking) {
                return false;
            } else {
                // else if the channel is non-blocking, cancel the existing
                // animation and start the new one.
                slot.Cancel();
                slot = Animation();
            }
        }

        slot = animation;
        return true;
    }

    void
    Cancel(AnimationChannel channel) {
        if (channel != AnimationChannel::Blocking) {
            const size_t index = static_cast<size_t>(channel);
            Animation &slot = slots[index];
            if (slot.Valid()) {
                slot.Cancel();
                slot = Animation();
            }
        }
    }

    void
    Update(void) {
        for (size_t i = 0; i < ChannelCount; i++) {
            if (!slots[i].Valid()) {
                continue;
            }
            if (!slots[i].Tick()) {
                slots[i] = Animation();
            }
        }
    }

    bool
    Busy(void) const {
        return slots[static_cast<size_t>(AnimationChannel::Blocking)].Valid();
    }

  private:
    Animation slots[ChannelCount];
};

/**
 * @brief Type-erased animation effect plugged into Animated.
 */
class AnimationEffect {
  public:
    using TryHandleFn = bool (*)(void *ctx, const Event &e,
                                 AnimationGate &gate);
    using SuppressFn = bool (*)(void *ctx, const Event &e);
    using IsActiveFn = bool (*)(void *ctx);
    using AfterHandleFn = void (*)(void *ctx, const Event &e,
                                   AnimationGate &gate);
    using CancelFn = void (*)(void *ctx);
    using SuppressesViewUpdateFn = bool (*)(void *ctx);

    AnimationEffect(void *ctx, TryHandleFn tryHandle, SuppressFn suppress,
                    IsActiveFn isActive, AfterHandleFn afterHandle,
                    CancelFn cancel,
                    SuppressesViewUpdateFn suppressesViewUpdate)
        : ctx(ctx), tryHandle(tryHandle), suppress(suppress),
          isActive(isActive), afterHandle(afterHandle), cancel(cancel),
          suppressesViewUpdate(suppressesViewUpdate) {}

    AnimationEffect(void)
        : AnimationEffect(nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                          nullptr) {}

    template <typename T>
    static AnimationEffect
    From(T &effect) {
        return AnimationEffect(
            &effect,
            [](void *ctx, const Event &e, AnimationGate &gate) -> bool {
                return static_cast<T *>(ctx)->TryHandle(e, gate);
            },
            [](void *ctx, const Event &e) -> bool {
                return static_cast<T *>(ctx)->Suppress(e);
            },
            [](void *ctx) -> bool { return static_cast<T *>(ctx)->IsActive(); },
            [](void *ctx, const Event &e, AnimationGate &gate) {
                static_cast<T *>(ctx)->AfterHandle(e, gate);
            },
            [](void *ctx) { static_cast<T *>(ctx)->Cancel(); },
            [](void *ctx) -> bool {
                return static_cast<T *>(ctx)->SuppressesViewUpdate();
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

    void
    AfterHandle(const Event &e, AnimationGate &gate) const {
        afterHandle(ctx, e, gate);
    }

    void
    Cancel(void) const {
        cancel(ctx);
    }

    bool
    SuppressesViewUpdate(void) const {
        return suppressesViewUpdate(ctx);
    }

  private:
    void *ctx;
    TryHandleFn tryHandle;
    SuppressFn suppress;
    IsActiveFn isActive;
    AfterHandleFn afterHandle;
    CancelFn cancel;
    SuppressesViewUpdateFn suppressesViewUpdate;
};

/**
 * @brief Per-view proxy: single Subscribe/Update entry, multiple Effect slots.
 */
template <typename View, size_t MaxEffectCount>
class TAnimated {
  public:
    TAnimated(View &view, AnimationGate &gate)
        : view(view), gate(gate), effectCount(0) {}

    bool
    Add(const AnimationEffect &effect) {
        if (effectCount >= MaxEffectCount) {
            return false;
        }

        effects[effectCount++] = effect;

        return true;
    }

    template <typename Effect>
    bool
    Add(Effect &effect) {
        return Add(AnimationEffect::From(effect));
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

        const EventResult result = view.HandleEvent(e);

        for (size_t i = 0; i < effectCount; i++) {
            effects[i].AfterHandle(e, gate);
        }

        return result;
    }

    /**
     * @brief Bypass view.Update() when an active effect draws the view itself.
     */
    void
    Update(void) {
        // If any active effect suppresses view update, do not update the view.
        for (size_t i = 0; i < effectCount; i++) {
            const auto &effect = effects[i];
            if (effect.IsActive() && effect.SuppressesViewUpdate()) {
                return;
            }
        }
        // Otherwise, update the view.
        view.Update();
    }

  private:
    View &view;
    AnimationGate &gate;
    AnimationEffect effects[MaxEffectCount];
    size_t effectCount;
};

template <typename View>
using Animated = TAnimated<View, MaxEffects>;

}; // namespace HexCalc
