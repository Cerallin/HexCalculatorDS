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

    InputEventData(EventDataType value) {
      data.digit = static_cast<Digit>(value & 0xFF);
      isOp = ((value & 0x100) != 0);
    }

    int
    ToInt() {
        return ((isOp ? 0x100 : 0) | (data.digit & 0xFF));
    }
};

static_assert(sizeof(InputEventData) <= sizeof(EventDataType),
              "InputEventData must fit in EventDataType");

class FormulaModel {
  public:
    explicit FormulaModel(EventBus &eventBus)
        : formulaTree(), bus(eventBus) {}

    EventResult HandleEvent(const Event &e);

    const FormulaTree &
    Tree(void) const {
        return formulaTree;
    }

  private:
    void NotifyChanged(void);

    FormulaTree formulaTree;
    EventBus &bus;
};

/**
 * @brief Number on the screen
 *
 */
class ValueModel {
  public:
    explicit ValueModel(EventBus &eventBus)
        : value(NumberZero), bus(eventBus) {}

    EventResult HandleEvent(const Event &e);

    NumberDataType
    Value(void) const {
        return value;
    }

  private:
    void NotifyChanged(void);

    NumberDataType value;
    EventBus &bus;
};

template <typename T>
class TranscodeModel {
  public:
    TranscodeModel(void) : value(NumberZero) {}

    EventResult HandleEvent(const Event &e);

    bool
    Selected(void) const {
        // TODO not implemented yet
        return false;
    }

  private:
    NumberDataType value;
};

}; // namespace HexCalc
