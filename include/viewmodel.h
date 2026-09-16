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

    NumberShiftMode GetShiftMode(void) const;

    NumberDataType
    GetRawValue(void) const {
        return formulaModel.CurrentNumber();
    }

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

/**
 * @brief Sliding window over formula glyphs (frontend-style page metadata).
 *
 * The window moves by pageSize each step, clamped to [0, max(0,
 * total-pageSize)]. Default view is pinned to the end (newest glyphs). Left
 * goes toward the start; right goes toward the end — so 12345 / pageSize 2
 * yields: left:  [45] -> [23] -> [12] right: [12] -> [34] -> [45]
 */
template <size_t MaxPageGlyphs>
struct FormulaPageInfo {
    GlyphArray6x8<MaxPageGlyphs> Glyphs;
    int Page;
    int TotalPages;
    size_t PageSize;
    size_t Total;
    size_t StartIndex;
    bool HasNext;
    bool HasPrevious;
};

class FormulaManager {
  public:
    FormulaManager(EventBus &eventBus, ValueManager &vm)
        : eventBus(eventBus), vm(vm), formulaGlyphs(), formulaState(Evaluated),
          currentNumber(NumberZero), leftBracketCount(0), windowStart(0),
          pinnedToEnd(true), collectingNumber(false) {}

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

    static constexpr size_t MaxTransitionGlyphs = 2 * MaxPageGlyphs;

    FormulaPageInfo<MaxPageGlyphs>
    GetFormulaPageAt(size_t startIndex) const {
        const size_t total = formulaGlyphs.Size();
        const size_t pageSize = MaxPageGlyphs;
        const size_t maxStart = maxWindowStart(total, pageSize);
        const size_t start = startIndex > maxStart ? maxStart : startIndex;
        const int totalPages =
            total == 0 ? 1
                       : static_cast<int>((total + pageSize - 1) / pageSize);
        const int page = (start >= maxStart)
                             ? totalPages
                             : static_cast<int>(start / pageSize) + 1;

        return FormulaPageInfo<MaxPageGlyphs>{
            GlyphArray6x8<MaxPageGlyphs>(formulaGlyphs, start, pageSize),
            page,
            totalPages,
            pageSize,
            total,
            start,
            start > 0,
            start < maxStart,
        };
    }

    FormulaPageInfo<MaxPageGlyphs>
    GetFormulaPage(void) const {
        return GetFormulaPageAt(effectiveWindowStart());
    }

    GlyphArray6x8<MaxTransitionGlyphs>
    GetFormulaSlice(size_t start, size_t count) const {
        return GlyphArray6x8<MaxTransitionGlyphs>(formulaGlyphs, start, count);
    }

    size_t
    GetWindowStart(void) const {
        return effectiveWindowStart();
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

    size_t windowStart;
    bool pinnedToEnd;

    bool collectingNumber;

    static size_t
    maxWindowStart(size_t total, size_t pageSize) {
        return total > pageSize ? total - pageSize : 0;
    }

    size_t
    effectiveWindowStart(void) const {
        const size_t maxStart =
            maxWindowStart(formulaGlyphs.Size(), MaxPageGlyphs);
        if (pinnedToEnd) {
            return maxStart;
        }
        return windowStart > maxStart ? maxStart : windowStart;
    }

    void
    pinWindowToEnd(void) {
        pinnedToEnd = true;
        windowStart = maxWindowStart(formulaGlyphs.Size(), MaxPageGlyphs);
    }

    void notifyFormulaUpdate(void);

    void formulaInsertOp(OperatorType op);
    void formulaInsertDigits();

    /**
     * @brief Slide the formula window left (older) or right (newer).
     *
     * @param dir DirLeft: start -= pageSize (floor 0); DirRight: start +=
     * pageSize (ceil maxStart)
     * @return true if the window moved
     */
    bool switchPage(Direction dir);

    void resetFormulaState(void);
};

class ViewManager {
  public:
    ViewManager(EventBus &eventBus, ViewModel &vm);

    EventResult HandleEvent(const Event &e);

  private:
    EventBus &eventBus;
    ViewModel &vm;
    NumberBase previousBase;
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

    NumberShiftMode
    GetShiftMode(void) const {
        return valueManager.GetShiftMode();
    }

    NumberDataType
    GetRawValue(void) const {
        return valueManager.GetRawValue();
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

    FormulaPageInfo<FormulaManager::MaxPageGlyphs>
    GetFormulaPageAt(size_t startIndex) const {
        return formulaManager.GetFormulaPageAt(startIndex);
    }

    FormulaPageInfo<FormulaManager::MaxPageGlyphs>
    GetFormulaPage(void) const {
        return formulaManager.GetFormulaPage();
    }

    GlyphArray6x8<FormulaManager::MaxTransitionGlyphs>
    GetFormulaSlice(size_t start, size_t count) const {
        return formulaManager.GetFormulaSlice(start, count);
    }

    size_t
    GetWindowStart(void) const {
        return formulaManager.GetWindowStart();
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

    /**
     * @brief Manager for view updates and synchronization
     *
     */
    ViewManager viewManager;
};

}; // namespace HexCalc
