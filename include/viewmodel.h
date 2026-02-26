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

namespace HexCalc {

class ViewModel;

class ValueManager {
  public:
    ValueManager(FormulaModel &formulaModel) : formulaModel(formulaModel) {}

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
        uint16_t byteNumber = (currentNumber >> (i * 16)) & 0xFFFF;
        Number number(byteNumber, width, sign);
        auto digits = number.Transcode<N>(base);

        return digits;
    }

  private:
    FormulaModel &formulaModel;
};

class FormulaPaginator {
  public:
    FormulaPaginator(EventBus &eventBus, ValueManager &vm)
        : eventBus(eventBus), vm(vm), formulaGlyphs(), formulaState(Evaluated),
          currentNumber(NumberZero), collectingNumber(false) {}

    EventResult HandleEvent(const Event &e);

    static constexpr size_t MaxFormulaGlyphs = 128;
    /**
     * @brief Padding for scrolling indicators when the formula exceeds the
     * display capacity.
     *
     */
    static constexpr int padding = 2;
    /**
     * @brief The maximum number of glyphs that can be displayed on the screen
     * at once, with padding for scrolling indicators.
     *
     */
    static constexpr size_t MaxPageGlyphs =
        (SCREEN_WIDTH / GlyphArray6x8<0>::CharWidth) - 2 * padding;

    /**
     * @brief Get the Formula Glyphs object for the current page of the formula
     *
     * @param page The page number starting from 1, from right to left
     * @return const GlyphArray6x8<MaxPageGlyphs> The glyphs for the specified
     * page of the formula
     */
    const GlyphArray6x8<MaxPageGlyphs>
    GetFormulaGlyphs(int page = 1) const {
        auto size = formulaGlyphs.Size();
        size_t offset =
            std::max(0, static_cast<ssize_t>(size) -
                            static_cast<ssize_t>(page * MaxPageGlyphs));
        GlyphArray6x8<MaxPageGlyphs> pageGlyphs(formulaGlyphs, offset,
                                                MaxPageGlyphs);
        return pageGlyphs;
    }

    size_t
    Size() const {
        return formulaGlyphs.Size();
    }

  private:
    EventBus &eventBus;

    ValueManager &vm;

    /**
     * @brief Glyphs for displaying the formula.
     *
     */
    GlyphArray6x8<MaxFormulaGlyphs> formulaGlyphs;

    static constexpr size_t MaxFormulaQueueSize = 32;

    static_assert(
        MaxFormulaQueueSize > (Number::MaxHexDigits + 4),
        "MaxFormulaQueueSize must be greater than MaxDisplayDigits + 4");

    enum {
        Evaluated,
        InputOp,
        InputDigit,
    } formulaState;

    NumberDataType currentNumber;

    bool collectingNumber;

    void notifyFormulaUpdate(void);

    void formulaInsertOp(OperatorType op);
    void formulaInsertDigits();
};

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

    NumberWidth
    GetNumberWidth(void) const {
        return valueManager.GetNumberWidth();
    }

    NumberSign
    GetNumberSign(void) const {
        return valueManager.GetNumberSign();
    }

    NumberBase
    GetNumberBase(void) const {
        return valueManager.GetNumberBase();
    }

    template <size_t N>
    DigitArray<N>
    GetValueDigits(NumberBase base) const {
        return valueManager.GetValueDigits<N>(base);
    }

    template <size_t N>
    DigitArray<N>
    GetValueDigitsPerByte(int i, NumberBase base) const {
        return valueManager.GetValueDigitsPerByte<N>(i, base);
    }

    const auto
    GetFormulaGlyphs() const {
        return formulaPaginator.GetFormulaGlyphs();
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
     * @brief Manager for current value and its digit representation
     *
     */
    ValueManager valueManager;

    /**
     * @brief Manager for formula glyphs
     *
     */
    FormulaPaginator formulaPaginator;

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
