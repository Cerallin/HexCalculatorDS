/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "animation.h"
#include "interpolation.h"
#include "viewhost.h"

namespace HexCalc::AnimationEffects {

/**
 * @brief Focus sign breathe: lerp sign palette between bg and border while a
 *        keyboard button is focused. Runs on the NonBlocking channel.
 */
class FocusSignBreathe {
  public:
    FocusSignBreathe(InputView &inputView, SubDisplay &display);

    bool TryHandle(const Event &e, AnimationGate &gate);
    bool Suppress(const Event &e) const;
    bool IsActive(void) const;
    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);
    bool SuppressesViewUpdate(void) const;

  private:
    static constexpr uint16_t Period = 60;

    InputView &inputView;
    SubDisplay &display;
    ColorCurve curve;

    bool active;
    int buttonIndex;
    uint16_t phase;

    void sync(AnimationGate &gate);
    bool Tick(void);

    static bool TickThunk(void *ctx);
    static void CancelThunk(void *ctx);
};

}; // namespace HexCalc::AnimationEffects
