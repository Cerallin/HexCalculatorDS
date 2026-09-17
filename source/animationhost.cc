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
      focusSignBreathe(views.GetInputViewAdapter().GetInputView(),
                       views.GetSubDisplay()),
      formulaAnimation(views.GetFormulaView(), gate),
      inputAnimation(views.GetInputViewAdapter(), gate) {
    bind();
    subscribe(vm.Bus());
}

void
AnimationHost::Update(void) {
    views.Update();
    formulaAnimation.Update();
    inputAnimation.Update();
    gate.Update();
}

void
AnimationHost::bind(void) {
    formulaAnimation.Add(formulaPageSlide);
    inputAnimation.Add(focusSignBreathe);
}

void
AnimationHost::subscribe(EventBus &bus) {
    bus.Subscribe(formulaAnimation);
    bus.Subscribe(inputAnimation);
}
