/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "commands.h"
#include "event.h"
#include "input.h"
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

    EventBus &
    Bus(void) {
        return eventBus;
    }

    Commands &
    Cmds(void) {
        return commands;
    }

    auto
    GetNumber(void) const {
        return valueModel.Value();
    }

  private:
    EventBus eventBus;

    /**
     * @brief Commands that can be triggered by user inputs
     *
     */
    Commands commands;

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
     * @brief Key input handler to identify key inputs and generate events
     *
     */
    KeyInputHandler keyInputHandler;

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
