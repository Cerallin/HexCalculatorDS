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
    ViewModel(void);

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
    EventBus &Bus(void);

    FormulaModel &Formula(void);
    const FormulaModel &Formula(void) const;

    ValueModel &Value(void);
    const ValueModel &Value(void) const;

  private:
    EventBus eventBus;

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
