/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"

namespace HexCalc {

enum CommandType : uint8_t {
    Invalid = 0xFF,
    InputNum0 = 0,
    InputNum1,
    InputNum2,
    InputNum3,
    InputNum4,
    InputNum5,
    InputNum6,
    InputNum7,
    InputNum8,
    InputNum9,
    InputNumA,
    InputNumB,
    InputNumC,
    InputNumD,
    InputNumE,
    InputNumF,
    InputOpPlus,
    InputOpMinus,
    InputOpMultiply,
    InputOpDivide,
    InputOpAnd,
    InputOpOr,
    InputOpModulo,
    InputOpLShift,
    InputOpRShift,
    InputOpBackspace,
    InputOpNegate,
    InputOpBitwiseNot,
    InputOpLBrac,
    InputOpRBrac,
};

class Commands {
  public:
    Commands(EventBus &eventBus) : bus(eventBus) {}

    void InputNumber0(void);
    void InputNumber1(void);
    void InputNumber2(void);
    void InputNumber3(void);
    void InputNumber4(void);
    void InputNumber5(void);
    void InputNumber6(void);
    void InputNumber7(void);
    void InputNumber8(void);
    void InputNumber9(void);
    void InputNumberA(void);
    void InputNumberB(void);
    void InputNumberC(void);
    void InputNumberD(void);
    void InputNumberE(void);
    void InputNumberF(void);
    void InputOperatorPlus(void);
    void InputOperatorMinus(void);
    void InputOperatorMultiply(void);
    void InputOperatorDivide(void);
    void InputOperatorAnd(void);
    void InputOperatorOr(void);
    void InputOperatorModulo(void);
    void InputOperatorLShift(void);
    void InputOperatorRShift(void);
    void InputOperatorClear(void);
    void InputOperatorBackspace(void);
    void InputOperatorNegate(void);
    void InputOperatorBitwiseNot(void);
    void InputOperatorEqual(void);
    void InputOperatorLBrac(void);
    void InputOperatorRBrac(void);

    void SwitchBaseUpper(void);
    void SwitchBaseLower(void);
    void SwitchWidth(void);

  private:
    EventBus &bus;
};

}; // namespace HexCalc
