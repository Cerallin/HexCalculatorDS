/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "input.h"
#include "commands.h"

using namespace HexCalc;

bool
KeyInputHandler::Handle(const KeyInput &input) {
    bool res = false;

    if (input.PressedUp()) {
        commands.SwitchBaseUpper();
        res = true;
    } else if (input.PressedDown()) {
        commands.SwitchBaseLower();
        res = true;
    } else if (input.PressedLeft()) {
        // do nothing for now
    } else if (input.PressedRight()) {
        // do nothing for now
    } else if (input.PressedA()) {
        // TODO input previous selected button
    } else if (input.PressedB()) {
        commands.InputOperatorBackspace();
        res = true;
    } else if (input.PressedX()) {
        commands.Clear();
        res = true;
    } else if (input.PressedY()) {
        // TODO switch width
    } else if (input.PressedStart()) {
        commands.InputOperatorEqual();
        res = true;
    } else if (input.PressedL()) {
        // TODO
    } else if (input.PressedR()) {
        // TODO
    }

    return res;
}
