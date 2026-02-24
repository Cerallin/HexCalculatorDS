/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "commands.h"
#include "event.h"
#include "format.h"
#include "input.h"
#include "model.h"

static constexpr size_t
max(size_t a, size_t b) {
    return (a > b) ? a : b;
}

static constexpr size_t
max(size_t a, size_t b, size_t c, size_t d) {
    return max(max(a, b), max(c, d));
}

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

    static constexpr size_t MaxDisplayDigits =
        max(Number::MaxBinDigits, Number::MaxOctDigits, Number::MaxDecDigits,
            Number::MaxHexDigits);

    static constexpr size_t MaxTranscodeDigits =
        max(MaxDigitsForType<Binary>(), MaxDigitsForType<Octal>(),
            MaxDigitsForType<Decimal>(), MaxDigitsForType<Hexadecimal>());

    auto
    GetNumber(void) const {
        return formulaModel.CurrentNumber();
    }

    NumberWidth GetNumberWidth(void) const;

    NumberSign GetNumberSign(void) const;

    NumberBase GetNumberBase(void) const;

    template <size_t N>
    DigitArray<N>
    GetValueDigits(void) const {
        auto base = GetNumberBase();
        auto sign = GetNumberSign();

        Number number(formulaModel.CurrentNumber(), sign);
        auto digits = number.Transcode<N>(base);

        return digits;
    }

    template <size_t N>
    DigitArray<N>
    GetValueDigitsPerByte(int i) const {
        assert(i >= 0 && i < 4); // valid byte index for 64-bit number
        auto base = GetNumberBase();
        auto sign = GetNumberSign();

        auto currentNumber = formulaModel.CurrentNumber();

        auto byteNumber = (currentNumber >> (i * 16)) & 0xFFFF;

        Number number(byteNumber, sign);
        auto digits = number.Transcode<N>(base);

        return digits;
    }

  private:
    EventBus eventBus;

    /**
     * @brief Commands that can be triggered by user inputs
     *
     */
    Commands commands;

    /**
     * @brief Formula tree and current input number
     *
     */
    FormulaModel formulaModel;

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
