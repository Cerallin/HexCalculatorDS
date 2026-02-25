/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "model.h"
#include "commands.h"
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
    } else if (e.type == EvaluateEvent) {
        debugf("input number: %llu\n", currentNumber);
        formulaTree.Input(FormulaData(currentNumber));
        bool evaluated = formulaTree.Evaluate();
        debugf("Evaluation result: %s\n", evaluated ? "OK" : "Error");
        if (evaluated) {
            currentNumber = formulaTree.Result();
        } else {
            currentNumber = NumberZero;
        }
        formulaTree.Clear();
        inputState = PlaceHolder;
        formulaChanged = true;
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

    if (eventData.isOp) { // is operator
        OperatorType op = eventData.data.op;
        // 1. insert current number into formula tree
        debugf("input number: %llu\n", currentNumber);
        formulaTree.Input(FormulaData(currentNumber));
        // 2. evaluate the formula tree
        auto lastOp = formulaTree.LastOperator();
        if (Operator::HigherThan(op, lastOp)) {
            // do not evaluate if current operator has higher precedence than
            // the previous one, otherwise it will cause wrong result for
            // expressions like "1 + 2 * 3"
        } else { // op <= lastOp
            bool evaluated = formulaTree.EvaluatePartial();
            debugf("Partial evaluation result: %s\n",
                   evaluated ? "OK" : "Error");
            if (evaluated) {
                currentNumber = formulaTree.Result();
                valueChanged = true;
            } else {
                // do not update current number if evaluation failed
            }
        }
        // 3. insert operator into formula tree
        // must after evaluation, otherwise the new operator will affect the
        // evaluation
        debugf("input op: %d\n", op);
        formulaTree.Input(FormulaData(op));
        formulaChanged = true;
        // 4. Mark the current number as placeholder, so that the next digit
        // input will start a new number instead of joining the current number.
        inputState = PlaceHolder;
    } else { // is number
        if (inputState == PlaceHolder) {
            // if the previous input is an operator, start a new number
            currentNumber = NumberZero;
            inputState = InputNumber;
        }

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
    constexpr auto maxWidthforBinary = DWord;
    if ((config.Base() == Binary) && (config.Width() > maxWidthforBinary)) {
        notifyWidthChange(maxWidthforBinary);
    } else {
        notifyValueChange();
    }
}
