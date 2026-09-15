/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "number.h"
#include "operator.h"

namespace HexCalc {

class Commands {
  public:
    explicit Commands(EventBus &eventBus) : bus(eventBus) {}

    void InputNumber(Digit digit);
    void InputOperator(OperatorType op);
    void InputOperatorBackspace(void);
    void InputPrevious(void);

    void FlipBit(int index);

    void Evaluate(void);

    void SwitchBaseUpper(void);
    void SwitchBaseLower(void);
    void SwitchWidthUpper(void);
    void SwitchWidthLower(void);

    void InputLeftShift(void);
    void InputRightShift(void);

    void ToggleSign(void);

    void SwitchShiftModeLeft(void);
    void SwitchShiftModeRight(void);

    void Clear(void);

    void SwitchFormulaPageLeft(void);
    void SwitchFormulaPageRight(void);

    void MoveFocusUp(void);
    void MoveFocusDown(void);
    void MoveFocusLeft(void);
    void MoveFocusRight(void);

  private:
    EventBus &bus;
};

}; // namespace HexCalc
