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

struct UpdateBaseEventData {
    NumberBase newBase;

    UpdateBaseEventData(int data) {
        if (data == Hexadecimal) {
            newBase = Hexadecimal;
        } else if (data == Decimal) {
            newBase = Decimal;
        } else if (data == Octal) {
            newBase = Octal;
        } else if (data == Binary) {
            newBase = Binary;
        }
    }
};

struct UpdateWidthEventData {
    NumberWidth newWidth;

    UpdateWidthEventData(int data) {
        if (data == QWord) {
            newWidth = QWord;
        } else if (data == DWord) {
            newWidth = DWord;
        } else if (data == Word) {
            newWidth = Word;
        } else if (data == Byte) {
            newWidth = Byte;
        }
    }
};

struct UpdateSignEventData {
    NumberSign newSign;

    UpdateSignEventData(int data) {
        if (data == Signed) {
            newSign = Signed;
        } else if (data == Unsigned) {
            newSign = Unsigned;
        }
    }
};

class Config {
  public:
    Config(void)
        : base(NumberBase::Decimal), width(NumberWidth::QWord), sign(Unsigned) {
    }

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

  private:
    NumberBase base;
    NumberWidth width;
    NumberSign sign;
};

extern Config config;

}; // namespace HexCalc
