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

    InputEventData(int value) {
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
    FormulaModel(void) : formulaTree() {}

    HandleEventResult HandleEvent(const Event &e);

  private:
    FormulaTree formulaTree;
};

/**
 * @brief Number on the screen
 *
 */
class DisplayModel {
  public:
    DisplayModel(void) : value(NumberZero) {}

    HandleEventResult HandleEvent(const Event &e);

  private:
    NumberDataType value;
};

template <typename T>
class TranscodeModel {
  public:
    TranscodeModel(void) : value(NumberZero) {}

    HandleEventResult HandleEvent(const Event &e);

    bool
    Selected(void) const {
        // TODO not implemented yet
        return false;
    }

  private:
    NumberDataType value;
};

}; // namespace HexCalc
