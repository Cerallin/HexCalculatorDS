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

#include <nds.h>

namespace HexCalc {

class ViewModel;

class ValueManager {
  public:
    explicit ValueManager(FormulaModel &formulaModel)
        : formulaModel(formulaModel) {}

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

template <size_t MaxFormulaGlyphs, size_t MaxPageGlyphs>
class PaginatedGlyphArray {
  public:
    PaginatedGlyphArray(const GlyphArray6x8<MaxFormulaGlyphs> &formulaGlyphs,
                        int page, size_t totalSize)
        : glyphs(formulaGlyphs,
                 std::max(0, static_cast<ssize_t>(formulaGlyphs.Size()) -
                                 static_cast<ssize_t>(page * MaxPageGlyphs)),
                 MaxPageGlyphs),
          page(page), totalSize(totalSize) {}

    const auto &
    Glyphs() const {
        return glyphs;
    }

    bool
    HasNextPage() const {
        return (page * MaxPageGlyphs) < totalSize;
    }

    bool
    HasPreviousPage() const {
        return page > 1;
    }

  private:
    GlyphArray6x8<MaxPageGlyphs> glyphs;
    int page;
    size_t totalSize;
};

class FormulaManager {
  public:
    FormulaManager(EventBus &eventBus, ValueManager &vm)
        : eventBus(eventBus), vm(vm), formulaGlyphs(), formulaState(Evaluated),
          currentNumber(NumberZero), leftBracketCount(0), currentPage(1),
          collectingNumber(false) {}

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
     * @brief Get the glyphs to be displayed for the current page of the formula
     *
     * @return const GlyphArray6x8<MaxPageGlyphs> The glyphs to be displayed for
     * the current page of the formula
     */
    const auto
    GetFormulaPaginator() const {
        return PaginatedGlyphArray<MaxFormulaGlyphs, MaxPageGlyphs>(
            formulaGlyphs, currentPage, formulaGlyphs.Size());
    }

    size_t
    Size(void) const {
        return formulaGlyphs.Size();
    }

    int
    GetLeftBracketCount() const {
        return leftBracketCount;
    }

  private:
    EventBus &eventBus;
    ValueManager &vm;

    /**
     * @brief Glyphs for displaying the formula.
     *
     */
    GlyphArray6x8<MaxFormulaGlyphs> formulaGlyphs;

    enum {
        Evaluated,
        InputOp,
        InputDigit,
        InputBracket,
    } formulaState;

    NumberDataType currentNumber;

    /**
     * @brief Count unclosed left brackets.
     *
     */
    int leftBracketCount;

    int currentPage;

    bool collectingNumber;

    void notifyFormulaUpdate(void);

    void formulaInsertOp(OperatorType op);
    void formulaInsertDigits();

    /**
     * @brief Switch to the next or previous page of the formula display, if
     * applicable.
     *
     * @param dir The direction to switch the page (DirLeft for next page,
     * DirRight for previous page)
     * @return true if the page was switched, false if there is no next/previous
     * page to switch to
     */
    bool switchPage(Direction dir);

    void resetFormulaState(void);
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
    ViewModel(EventBus &eventBus, Commands &commands);

    /**
     * @brief Dispatch events from event queue to models and views
     *
     */
    void DispatchEvents(void);

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
    GetFormulaPaginator() const {
        return formulaManager.GetFormulaPaginator();
    }

    auto
    GetLeftBracketCount() const {
        return formulaManager.GetLeftBracketCount();
    }

  private:
    /**
     * @brief Event bus for communication between input, models and views
     *
     */
    EventBus &eventBus;

    /**
     * @brief Commands that can be triggered by user inputs
     *
     */
    Commands &commands;

    /**
     * @brief Formula tree and current input number
     *
     */
    FormulaModel formulaModel;

    /**
     * @brief Manager for current value and its digit representation
     *
     */
    ValueManager valueManager;

    /**
     * @brief Manager for formula glyphs
     *
     */
    FormulaManager formulaManager;
};

}; // namespace HexCalc
