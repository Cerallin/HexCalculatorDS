/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "model.h"

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
        auto eventData = static_cast<InputEventData>(e.data);

        if (eventData.isOp) {
            // do nothing
        } else {
            value = value * 16 + eventData.data.digit;
            changed = true;
        }
    } else if (e.type == ClearEvent) {
        value = NumberZero;
        changed = true;
    } else if (e.type == EvaluateEvent) {
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
