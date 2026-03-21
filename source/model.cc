/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "model.h"
#include "commands.h"
#include "config.h"
#include "format.h"

using namespace HexCalc;

EventResult
FormulaModel::HandleEvent(const Event &e) {
    bool valueChanged = false;
    bool formulaChanged = false;

    if (e.type == BackspaceEvent) {
        if (inputState == PlaceHolder) {
            currentNumber = NumberZero;
            inputState = InputNumber;
        } else { // inputState == InputNumber
            // remove the last digit
            auto base = config.Base();
            currentNumber /= base;
        }
        valueChanged = true;
    } else if (e.type == InputEvent) {
        return handleInput(e);
    } else if (e.type == ClearEvent) {
        if (inputState == PlaceHolder && currentNumber == NumberZero) {
            formulaTree.Clear();
            formulaChanged = true;
        }
        inputState = PlaceHolder;
        currentNumber = NumberZero;
        valueChanged = true;
    } else if (e.type == EvaluateEvent) {
        debugf("input number: %llu\n", currentNumber);
        bool inserted = formulaTree.Input(FormulaData(currentNumber));
        if (inserted) {
            notifyAcceptNumber(currentNumber);
        }
        // TODO get error message if insertion failed
        FormulaEvaluateResult evalRes = formulaTree.Evaluate();
        bool evaluateOK = (evalRes == EvalSuccess);
        debugf("Evaluation result: %s\n", evaluateOK ? "OK" : "Error");
        if (evaluateOK) {
            currentNumber = formulaTree.Result();
            notifyAcceptOperator(OperatorType::Equal);
            valueChanged = true;
        } else {
            currentNumber = NumberZero;
            notifyEvaluateError(evalRes);
            // do not update the formula view if evaluation failed
            valueChanged = false;
        }

        formulaTree.Clear();
        inputState = PlaceHolder;

        formulaChanged = true;
    } else if (e.type == UpdateBaseEvent) {
        handleBaseChange(e);
    } else if (e.type == UpdateWidthEvent) {
        auto oldValue = currentNumber;
        currentNumber &= WidthMask(config.Width());
        if (currentNumber != oldValue) {
            valueChanged = true;
        }
    } else {
        // Skip unhandled event type
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
FormulaModel::notifyAcceptOperator(OperatorType op) {
    bus.Post(Event{op, OperatorAcceptEvent});
}

// EvaluateErrorEvent
void
FormulaModel::notifyEvaluateError(FormulaEvaluateResult evalRes) {
    bus.Post(Event{static_cast<EventDataType>(evalRes), EvaluateErrorEvent});
}

void
FormulaModel::notifyAcceptNumber(NumberDataType number) {
    // Split into two 32-bit chunks; keep raw bits through int32 events.
    auto low32 = static_cast<uint32_t>(number & WidthMask(DWord));
    auto high32 = static_cast<uint32_t>((number >> DWord) & WidthMask(DWord));
    bus.Post(Event{static_cast<EventDataType>(high32), NumberAcceptEvent});
    bus.Post(Event{static_cast<EventDataType>(low32), NumberAcceptEvent});
}

EventResult
FormulaModel::handleInput(const Event &e) {
    bool valueChanged = false;
    bool formulaChanged = false;

    InputEventData eventData(e.data);

    if (eventData.isOp) { // is operator
        OperatorType op = eventData.data.op;
        // bitwise not and negate can be applied immediately without waiting
        // for the next number input, so handle them separately here.
        if (op == BitwiseNot) {
            currentNumber = ~currentNumber;
            currentNumber &= WidthMask(config.Width());
            inputState = InputNumber;
            valueChanged = true;
        } else if (op == Negate) {
            auto width = config.Width();
            auto widthMask = WidthMask(width);
            // Two's complement negation: invert all bits and add 1, then
            // apply width mask to fit the number into the current width.
            currentNumber = ((currentNumber ^ widthMask) + 1) & widthMask;
            inputState = InputNumber;
            valueChanged = true;
        } else if (op == LeftBracket) {
            if (inputState == PlaceHolder) {
                bool inserted = formulaTree.Input(FormulaData(op));
                if (inserted) {
                    notifyAcceptOperator(op);
                    formulaChanged = true;
                }
            } else {
                bool inserted = true;
                // 1. insert current number into formula tree
                debugf("input number: %llu\n", currentNumber);
                inserted &= formulaTree.Input(FormulaData(currentNumber));
                if (inserted) {
                    notifyAcceptNumber(currentNumber);
                    formulaChanged = true;
                }
                // 2. insert multiplication operator into formula tree, so
                // that '2 (' will be treated as '2 x ('
                inserted &=
                    formulaTree.Input(FormulaData(OperatorType::Multiply));
                if (inserted) {
                    notifyAcceptOperator(OperatorType::Multiply);
                    formulaChanged = true;
                }
                // 3. insert left bracket into formula tree
                inserted &= formulaTree.Input(FormulaData(op));
                if (inserted) {
                    notifyAcceptOperator(op);
                    formulaChanged = true;
                }
                currentNumber = NumberZero;
                inputState = PlaceHolder;
                valueChanged = true;
            }
        } else {
            // continue to handle other operators

            bool inserted;
            // 1. insert current number into formula tree
            debugf("input number: %llu\n", currentNumber);
            // this may fail, but does not matter, because even if the current
            // number cannot be inserted. e.g. '(1)' + '2' is invalid because
            // a number cannot be directly after a right bracket, while
            // '(1)' + 'x' -> '(1) x' is valid.
            inserted = formulaTree.Input(FormulaData(currentNumber));
            if (inserted) {
                notifyAcceptNumber(currentNumber);
                formulaChanged = true;
            }
            // 2. evaluate the formula tree
            auto lastOp = formulaTree.LastOperator();
            if (Operator::HigherThan(op, lastOp)) {
                // do not evaluate if current operator has higher
                // precedence than the previous one, otherwise it will
                // cause wrong result for expressions like "1 + 2 * 3"
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
            // must after evaluation, otherwise the new operator will
            // affect the evaluation
            debugf("input op: %d\n", op);
            inserted = formulaTree.Input(FormulaData(op));
            if (inserted) {
                notifyAcceptOperator(op);
                formulaChanged = true;
            }
            // 4. Mark the current number as placeholder, so that the
            // next digit input will start a new number instead of
            // joining the current number.
            inputState = PlaceHolder;
        }
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

        auto digit = eventData.data.digit;

        auto maxValue = NumberMax(width, sign);
        bool willOverflow = (currentNumber > (maxValue - digit) / base);

        if (willOverflow) {
            // reject
        } else {
            // join the new digit to the current number
            currentNumber = (currentNumber * base) + digit;
            valueChanged = true;
        }
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
FormulaModel::handleBaseChange(const Event &e) {
    constexpr auto maxWidthforBinary =
        (GlyphFormatSize<Binary>(QWord) < MaxDisplayDigits)   ? QWord
        : (GlyphFormatSize<Binary>(DWord) < MaxDisplayDigits) ? DWord
        : (GlyphFormatSize<Binary>(Word) < MaxDisplayDigits)  ? Word
                                                              : Byte;
    if ((config.Base() == Binary) && (config.Width() > maxWidthforBinary)) {
        notifyWidthChange(maxWidthforBinary);
    } else {
        notifyValueChange();
    }
}
