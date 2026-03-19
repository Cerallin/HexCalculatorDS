/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "display.h"
#include "view.h"
#include "viewmodel.h"

namespace HexCalc {

/**
 * @brief The ViewHost class manages the views on the main and sub screens. It
 * is responsible for updating the views when the models change and handling
 * user inputs by dispatching events to the appropriate views. It also
 * initializes the displays and registers the views with the ViewModel.
 *
 */
class ViewHost : private NonCopyable {
  public:
    explicit ViewHost(ViewModel &viewModel);

    void Update(void);

  private:
    MainDisplay mainDisplay;
    SubDisplay subDisplay;

    ConfigView configView;
    FormulaView formulaView;
    ValueView valueView;
    TranscodeView<Hexadecimal> hexView;
    TranscodeView<Decimal> decView;
    TranscodeView<Octal> octView;
    TranscodeView<Binary> binView;
    IndicatorView indicatorView;
    InputView inputView;

    void registerViews(ViewModel &viewModel);
};

}; // namespace HexCalc
