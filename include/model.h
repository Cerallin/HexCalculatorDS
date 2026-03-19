/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "formula.h"

namespace HexCalc {

/**
 * @brief The data structure for input events, which can represent either a
 * digit or an operator.
 *
 */
struct InputEventData {
    union {
        Digit digit;
        OperatorType op;
    } data;
    bool isOp;

    explicit constexpr InputEventData(Digit digit)
        : data{.digit = digit}, isOp(false) {}

    explicit constexpr InputEventData(OperatorType op)
        : data{.op = op}, isOp(true) {}

    explicit constexpr InputEventData(EventDataType value)
        : data{.digit = static_cast<Digit>(value & 0xFF)},
          isOp((value & 0x100) != 0) {}

    int
    ToInt(void) const {
        return ((isOp ? 0x100 : 0) | (data.digit & 0xFF));
    }
};

static_assert(sizeof(InputEventData) <= sizeof(EventDataType),
              "InputEventData must fit in EventDataType");

class FormulaModel {
  public:
    explicit FormulaModel(EventBus &eventBus)
        : bus(eventBus), formulaTree(), inputState(PlaceHolder),
          currentNumber(NumberZero) {}

    EventResult HandleEvent(const Event &e);

    const FormulaTree &
    Tree(void) const {
        return formulaTree;
    }

    NumberDataType
    CurrentNumber(void) const {
        return currentNumber;
    }

  private:
    EventBus &bus;
    FormulaTree formulaTree;

    enum InputState {
        PlaceHolder,
        InputNumber,
    } inputState;

    NumberDataType currentNumber;

    void notifyFormulaUpdate(void);
    void notifyValueChange(void);
    void notifyWidthChange(NumberWidth width);
    void notifyAcceptOperator(OperatorType op);
    void notifyAcceptNumber(NumberDataType number);

    EventResult handleInput(const Event &e);
    void handleBaseChange(const Event &e);
};

}; // namespace HexCalc
