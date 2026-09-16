/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "animation.h"
#include "formulapageslide.h"
#include "viewhost.h"

#include <type_traits>

namespace HexCalc {

/**
 * @brief Compile-time set of views owned by AnimationHost
 * (skip raw Subscribe/Update).
 */
struct AnimationViewClaims {
    template <typename V>
    struct IsClaimed : std::false_type {};
};

template <>
struct AnimationViewClaims::IsClaimed<FormulaView> : std::true_type {};

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

    bool
    IsAnimating(void) const {
        return gate.Busy();
    }

    void Update(void);

  private:
    Views &views;
    AnimationGate gate;
    AnimationEffects::FormulaPageSlide formulaPageSlide;
    Animated<FormulaView> formulaAnim;
};

}; // namespace HexCalc
