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

    EventResult HandleEvent(const Event &e);

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

    NumberWidth GetNumberWidth(void) const;

    NumberSign GetNumberSign(void) const;

    NumberBase GetNumberBase(void) const;

    template <size_t N>
    DigitArray<N>
    GetValueDigits(NumberBase base) const {
        auto sign = GetNumberSign();
        auto width = GetNumberWidth();

        Number number(formulaModel.CurrentNumber(), width, sign);
        auto digits = number.Transcode<N>(base);

        return digits;
    }

    template <size_t N>
    DigitArray<N>
    GetValueDigitsPerByte(int i, NumberBase base) const {
        assert(i >= 0 && i < 4); // valid byte index for 64-bit number

        auto sign = GetNumberSign();
        auto width = GetNumberWidth();

        auto currentNumber = formulaModel.CurrentNumber();
        auto byteNumber = (currentNumber >> (i * 16)) & 0xFFFF;
        Number number(byteNumber, width, sign);
        auto digits = number.Transcode<N>(base);

        return digits;
    }

    static constexpr size_t MaxFormulaGlyphs = 128;

    const GlyphArray6x8<MaxFormulaGlyphs> &
    GetFormulaGlyphs() const {
        return formulaGlyphs;
    }

  private:
    /**
     * @brief Event bus for communication between input, models and views
     *
     */
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
     * @brief Glyphs for displaying the formula
     *
     */
    GlyphArray6x8<MaxFormulaGlyphs> formulaGlyphs;
    CircularQueue<Glyph, MaxDisplayDigits> formulaGlyphQueue;

    bool lastGlyphWasOperator;

    void notifyFormulaUpdate(void);

    void formulaInsertOp(OperatorType op);
    void formulaInsertDigit(Digit digit);

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
