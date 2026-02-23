/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "model.h"
#include "config.h"

using namespace HexCalc;

EventResult
FormulaModel::HandleEvent(const Event &e) {
    bool valueChanged = false;
    bool formulaChanged = false;

    if (e.type == InputEvent) {
        handleInput(e);
        return Consumed;
    } else if (e.type == ClearEvent) {
        if (inputState == PlaceHolder && currentNumber == NumberZero) {
            formulaTree.Clear();
            formulaChanged = true;
        }
        inputState = InputNumber;
        currentNumber = NumberZero;
        valueChanged = true;
    } else if (e.type == FormulaEvaluateEvent) {
        // TODO
        valueChanged = true;
    } else if (e.type == UpdateBaseEvent) {
        handleBaseChange(e);
        return Consumed;
    } else if (e.type == UpdateWidthEvent) {
        auto oldValue = currentNumber;
        currentNumber &= WidthMask(config.Width());
        if (currentNumber != oldValue) {
            notifyValueChange();
        }
        valueChanged = true;
    } else {
        return Skipped;
    }

    if (formulaChanged) {
        notifyFormulaUpdate();
    }

    if (valueChanged) {
        notifyValueChange();
    }

    return Consumed;
}

void
FormulaModel::notifyFormulaUpdate(void) {
    bus.Post(Event{0, FormulaUpdatedEvent});
}

void
FormulaModel::notifyValueChange(void) {
    bus.Post(Event{0, ValueChangedEvent});
}

void
FormulaModel::notifyWidthChange(NumberWidth width) {
    bus.Post(Event{width, UpdateWidthEvent});
}

void
FormulaModel::handleInput(const Event &e) {
    bool valueChanged = false;
    bool formulaChanged = false;

    auto eventData = InputEventData(e.data);

    if (eventData.isOp) {
        // is operator
        // insert current number into formula tree
        formulaTree.Input(FormulaData(currentNumber));
        // insert operator into formula tree
        formulaTree.Input(FormulaData(eventData.data.op));
        formulaChanged = true;
        // update current number
        bool evaluated = formulaTree.EvaluatePartial();
        if (evaluated) {
            currentNumber = formulaTree.Result();
            inputState = PlaceHolder;
            valueChanged = true;
        } else {
            // do not update current number if evaluation failed
            inputState = InputNumber;
        }
    } else {
        // is number

        // check overflow
        auto base = config.Base();
        auto width = config.Width();
        auto sign = config.Sign();

        auto maxValue = NumberMax(width, sign);
        bool willOverflow = (currentNumber > maxValue / base) ||
                            (currentNumber == maxValue / base &&
                             eventData.data.digit > maxValue % base);

        if (willOverflow) {
            // reject
        } else {
            // join the new digit to the current number
            currentNumber = (currentNumber * base) + eventData.data.digit;
            valueChanged = true;
        }
    }

    if (formulaChanged) {
        notifyFormulaUpdate();
    }

    if (valueChanged) {
        notifyValueChange();
    }
}

void
FormulaModel::handleBaseChange(const Event &e) {
    constexpr auto maxDisplayDigits = Number::MaxDisplayDigits;
    if ((config.Base() == Binary) && (config.Width() > maxDisplayDigits)) {
        constexpr auto width = (QWord < maxDisplayDigits)   ? QWord
                               : (DWord < maxDisplayDigits) ? DWord
                               : (Word < maxDisplayDigits)  ? Word
                                                            : Byte;
        notifyWidthChange(width);
    } else {
        notifyValueChange();
    }
}
