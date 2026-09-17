/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationhost.h"

using namespace HexCalc;

AnimationHost::AnimationHost(Views &views, ViewModel &vm)
    : views(views), gate(), formulaPageSlide(views.GetFormulaView()),
      formulaAnimation(views.GetFormulaView(), gate) {
    bind();
    subscribe(vm.Bus());
}

void
AnimationHost::Update(void) {
    gate.Update();
    views.Update();
    formulaAnimation.Update();
}

void
AnimationHost::bind(void) {
    // Slide the formula by page
    formulaAnimation.Add(formulaPageSlide);
}

void
AnimationHost::subscribe(EventBus &bus) {
    bus.Subscribe(formulaAnimation);
}
