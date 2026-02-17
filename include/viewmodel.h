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

/**
 * @brief The ViewModel class manages the state of the application, including
 * the formula and value models, and the views that display them. It also
 * handles user inputs and dispatches events to update the models and views
 * accordingly.
 *
 */
class ViewModel : private NonCopyable {
  public:
    using ViewAlign = MainView::ViewAlign;

    ViewModel(void)
        : // models
          formulaModel(), valueModel(),
          // main screen views
          formulaView(Area(3, 0, 30, 1), ViewAlign::AlignRight),
          valueView(Area(5, 0, 30, 1), ViewAlign::AlignRight),
          hexView(Area(7, 0, 30, 1), ViewAlign::AlignLeft),
          decView(Area(9, 0, 30, 1), ViewAlign::AlignLeft),
          octView(Area(11, 0, 30, 1), ViewAlign::AlignLeft),
          binView(Area(13, 0, 30, 1), ViewAlign::AlignLeft),
          // subscreen view
          inputView(),
          // input state
          previousKeys(0) {}

    /**
     * @brief Dispatch events from event queue to models and views
     *
     */
    void DispatchEvents(void);
    /**
     * @brief Handle user inputs and generate events for models
     *
     */
    void HandleInputs(void);
    /**
     * @brief Update views according to the current state of models
     *
     */
    void UpdateViews(void);

  private:
    /**
     * @brief Formula tree
     *
     */
    FormulaModel formulaModel;
    /**
     * @brief Curent input number or calculated result
     *
     */
    ValueModel valueModel;

    /**
     * @brief A single line of the formula on the main screen
     *
     */
    FormulaView formulaView;
    /**
     * @brief Curent input or calculated result
     *
     */
    ValueView valueView;
    /**
     * @brief Hexadecimal representation of value
     *
     */
    TranscodeView<Hexadecimal> hexView;
    /**
     * @brief Decimal representation of value
     *
     */
    TranscodeView<Decimal> decView;
    /**
     * @brief Octal representation of value
     *
     */
    TranscodeView<Octal> octView;
    /**
     * @brief Binary representation of value
     *
     */
    TranscodeView<Binary> binView;

    /**
     * @brief Input view on subscreen
     *
     */
    InputView inputView;

    /**
     * @brief Previously input states
     *
     */
    uint32_t previousKeys;
    // TODO previousTouch;

    /**
     * @brief Identify key inputs and generate events
     *
     * @return true if key pressed, false if no keys are pressed
     */
    bool handleKeyInputs(void);
    /**
     * @brief Handle touch screen with input view
     *
     * @return true if a button is touched, false otherwise
     */
    bool handleTouchScreen(void);
};

}; // namespace HexCalc
