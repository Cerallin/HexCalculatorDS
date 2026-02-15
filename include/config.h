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

class Config {
  public:
    Config(void) : base(NumberBase::Decimal), width(NumberWidth::QWord) {}

    HandleEventResult HandleEvent(const Event &e);

  private:
    NumberBase base;
    NumberWidth width;
};

extern Config config;

}; // namespace HexCalc
