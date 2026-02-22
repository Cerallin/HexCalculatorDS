/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "model.h"
#include "config.h"

using namespace HexCalc;

/**
 * @brief The result of the last evaluation.
 */
uint64_t evaluateResult;

EventResult
FormulaModel::HandleEvent(const Event &e) {
    bool changed = false;
    if (e.type == InputEvent) {
        auto eventData = static_cast<InputEventData>(e.data);

        if (eventData.isOp) {
            formulaTree.Input(FormulaData(eventData.data.op));
        } else {
            formulaTree.Input(FormulaData(eventData.data.digit));
        }
        changed = true;
    } else if (e.type == ClearEvent) {
        formulaTree.Clear();
        changed = true;
    } else if (e.type == EvaluateEvent) {
        changed = formulaTree.Evaluate();
    }

    if (changed) {
        notifyChanged();
    }

    return Consumed;
}

void
FormulaModel::notifyChanged(void) {
    bus.Post(Event{0, FormulaChangedEvent});
}

EventResult
ValueModel::HandleEvent(const Event &e) {
    bool changed = false;
    if (e.type == InputEvent) {
        auto eventData = InputEventData(e.data);

        if (eventData.isOp) {
            // TODO implement after formula tree is done
        } else {
            // check overflow
            auto base = config.Base();
            auto width = config.Width();
            auto sign = config.Sign();

            auto maxValue = NumberMax(width, sign);
            bool willOverflow = (value > maxValue / base) ||
                                (value == maxValue / base &&
                                 eventData.data.digit > maxValue % base);

            if (willOverflow) {
                // do nothing
            } else {
                value = value * base + eventData.data.digit;
                changed = true;
            }
        }
    } else if (e.type == ClearEvent) {
        value = NumberZero;
        changed = true;
    } else if (e.type == EvaluateEvent) {
        // TODO evaluate the formula and update the value
    } else if (e.type == UpdateBaseEvent) {
        constexpr auto maxDigits = Number::MaxDecDigits;
        if ((config.Base() == Binary) && (config.Width() > maxDigits)) {
            constexpr auto width = (QWord < maxDigits)   ? QWord
                                   : (DWord < maxDigits) ? DWord
                                   : (Word < maxDigits)  ? Word
                                                         : Byte;
            bus.Post(Event{width, UpdateWidthEvent});
        } else {
            changed = true;
        }
    } else if (e.type == UpdateWidthEvent) {
        auto oldValue = value;
        value &= WidthMask(config.Width());
        if (value != oldValue) {
            notifyChanged();
        }
        changed = true;
    } else {
        // do nothing
    }

    if (changed) {
        notifyChanged();
    }

    return Consumed;
}

void
ValueModel::notifyChanged(void) {
    bus.Post(Event{0, ValueChangedEvent});
}
