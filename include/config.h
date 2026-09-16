/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "number.h"

namespace HexCalc {

class ConfigModel {
  public:
    ConfigModel(void)
        : base(NumberBase::Decimal), width(NumberWidth::QWord), sign(Unsigned),
          shiftMode(ArithmeticMode) {}

    EventResult HandleEvent(const Event &e);

    NumberBase
    Base(void) const {
        return base;
    }

    NumberWidth
    Width(void) const {
        return width;
    }

    NumberSign
    Sign(void) const {
        return sign;
    }

    NumberShiftMode
    ShiftMode(void) const {
        return shiftMode;
    }

  private:
    NumberBase base;
    NumberWidth width;
    NumberSign sign;
    NumberShiftMode shiftMode;
};

extern ConfigModel config;

}; // namespace HexCalc
