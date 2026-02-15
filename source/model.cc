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

HandleEventResult
FormulaModel::HandleEvent(const Event &e) {
    if (e.type == InputEvent) {
        auto eventData = static_cast<InputEventData>(e.data);

        if (eventData.isOp) {
            formulaTree.Input(FormulaData(eventData.data.op));
        } else {
            formulaTree.Input(FormulaData(eventData.data.digit));
        }
    } else if (e.type == ClearEvent) {
        formulaTree.Clear();
    } else if (e.type == EvaluateEvent) {
        if (!formulaTree.Evaluate()) {
            // do nothing
        }
    }

    return Handled;
}

HandleEventResult
DisplayModel::HandleEvent(const Event &e) {
    if (e.type == InputEvent) {
        auto eventData = static_cast<InputEventData>(e.data);

        if (eventData.isOp) {
            // do nothing
        } else {
            value = value * 16 + eventData.data.digit;
        }
    } else if (e.type == ClearEvent) {
        value = NumberZero;
    } else if (e.type == EvaluateEvent) {
        // do nothing
    }

    return Handled;
}
