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
      formulaAnim(views.GetFormulaView(), gate) {
    formulaAnim.Add(formulaPageSlide);
    vm.Bus().Subscribe(formulaAnim);
}

void
AnimationHost::Update(void) {
    gate.Update();
    views.Update();
    formulaAnim.Update();
}
