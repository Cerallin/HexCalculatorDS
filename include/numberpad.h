/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

namespace HexCalc {

template <size_t ColNum, size_t RowNum>
class NumberPad : public NonCopyable {
  public:
    explicit NumberPad(SubDisplay &display) {}

  private:
};

} // namespace HexCalc
