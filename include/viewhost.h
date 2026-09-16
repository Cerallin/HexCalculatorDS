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
 * @brief The InputViewAdapter class manages the views on the sub screen.
 *
 */
class InputViewAdapter : private SubView<InputViewAdapter> {
  public:
    explicit InputViewAdapter(SubDisplay &subDisplay, ViewModel &viewModel);

    /**
     * @brief Update the currently active view.
     *
     */
    void Update(void);

    EventResult HandleEvent(const Event &e);

  private:
    InputView inputView;
    EditorView editorView;
    DrawerView drawerView;

    enum {
        InputState,
        EditorState,
    } state;

    bool shouldSwitchView;
};

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
    InputViewAdapter inputViewAdapter;

    void registerViews(ViewModel &viewModel);
};

}; // namespace HexCalc
