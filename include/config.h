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

struct UpdateConfigEventData {

    NumberBase newBase;
    NumberWidth newWidth;
    NumberSign newSign;

    static constexpr int BaseBits = 2;
    static constexpr int WidthBits = 2;
    static constexpr int SignBits = 1;

    UpdateConfigEventData(int value) { FromInt(value); }

    constexpr int
    ToInt() const {
        return (static_cast<int>(newBase) & 0x3) |
               ((static_cast<int>(newWidth) & 0x3) << BaseBits) |
               ((static_cast<int>(newSign) & 0x1) << (BaseBits + WidthBits));
    }

    constexpr void
    FromInt(int value) {
        newBase = static_cast<NumberBase>(value & 0x3);
        newWidth = static_cast<NumberWidth>((value >> BaseBits) & 0x3);
        newSign =
            static_cast<NumberSign>((value >> (BaseBits + WidthBits)) & 0x1);
    }
};

class Config {
  public:
    Config(void)
        : base(NumberBase::Decimal), width(NumberWidth::QWord), sign(Unsigned) {
    }

    HandleEventResult HandleEvent(const Event &e);

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

  private:
    NumberBase base;
    NumberWidth width;
    NumberSign sign;
};

extern Config config;

}; // namespace HexCalc
