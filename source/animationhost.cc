/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationhost.h"

using namespace HexCalc;

AnimationHost::AnimationHost(Views &views, ViewModel &vm)
    : views(views), gate(), digitFocusAnimState(),
      formulaPageSlide(views.GetFormulaView()),
      focusSignBreathe(views.GetInputViewAdapter().GetInputView(),
                       views.GetSubDisplay()),
      shiftModeSlide(views.GetInputViewAdapter().GetEditorView()),
      digitFocusConverge(views.GetInputViewAdapter().GetEditorView(),
                         digitFocusAnimState),
      digitFocusDiverge(views.GetInputViewAdapter().GetEditorView(),
                        digitFocusAnimState),
      digitFocusSlide(views.GetInputViewAdapter().GetEditorView(),
                      digitFocusAnimState, digitFocusDiverge),
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
    inputAnimation.Add(shiftModeSlide);
    inputAnimation.Add(digitFocusConverge);
    inputAnimation.Add(digitFocusSlide);
    inputAnimation.Add(digitFocusDiverge);
}

void
AnimationHost::subscribe(EventBus &bus) {
    bus.Subscribe(formulaAnimation);
    bus.Subscribe(inputAnimation);
}
