/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "event.h"
#include "formula.h"
#include "model.h"
#include "view.h"

namespace HexCalc {

class ViewModel : private NonCopyable {
  public:
    ViewModel(void)
        : mainView(), subView(), formulaModel(), displayModel(),
          previousKeys(0) {}

    void DispatchEvents(void);
    void HandleInputs(void);
    void UpdateViews(void);

  private:
    // views
    HexCalc::MainView mainView;
    HexCalc::SubView subView;
    // models
    FormulaModel formulaModel;
    DisplayModel displayModel;

    // input
    uint32_t previousKeys;
};

}; // namespace HexCalc
