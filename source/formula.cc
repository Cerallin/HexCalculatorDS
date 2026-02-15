/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "formula.h"
#include "common.h"

using namespace HexCalc;

bool
FormulaTree::Input(const FormulaData &data) {
    if (size >= MaxSize) { // tree is full
        return false;
    }
    nodes[size++].Assign(data);

    return true;
}

void
FormulaTree::Clear(void) {
    auto rootNode = root();

    rootNode.Reset();
    rootNode.Assign(FormulaData(NumberZero));

    // Reset counter
    size = 0;
}

bool
HexCalc::FormulaTree::Evaluate(void) {
    // TODO not implemented yet
    return false;
}
