/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "animation.h"
#include "focussignbreathe.h"
#include "formulapageslide.h"
#include "viewhost.h"

namespace HexCalc {

/**
 * @brief Views owned by AnimationHost (skip raw Subscribe/Update).
 *        Extend with more types: ViewClaims<FormulaView, ValueView, ...>
 */
using AnimationViewClaims = ViewClaims<FormulaView, InputViewAdapter>;

/**
 * @brief Optional animation layer over ViewHost.
 *
 * Owns AnimationGate and Animated wrappers, binds them to the event bus, and
 * exposes IsAnimating for the main loop. Removing this host and using
 * ViewHost<> alone disables animation.
 */
class AnimationHost : private NonCopyable {
  public:
    using Views = ViewHost<AnimationViewClaims>;

    AnimationHost(Views &views, ViewModel &vm);

    /**
     * @brief Check if the animation is blocking.
     *
     * @return true if the animation is blocking, false otherwise.
     */
    bool
    IsBlocking(void) const {
        return gate.Busy();
    }

    void Update(void);

  private:
    Views &views;
    AnimationGate gate;

    // Animation effects
    AnimationEffects::FormulaPageSlide formulaPageSlide;
    AnimationEffects::FocusSignBreathe focusSignBreathe;

    // Animation wrappers
    Animated<FormulaView> formulaAnimation;
    Animated<InputViewAdapter> inputAnimation;

    /*
     * @brief Bind animation effects to the animation wrapper.
     */
    void bind(void);

    /*
     * @brief Subscribe to the animation wrapper.
     */
    void subscribe(EventBus &bus);
};

}; // namespace HexCalc
