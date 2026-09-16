/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "common.h"

#include "config.h"
#include "input.h"

using namespace HexCalc;

ConfigModel HexCalc::config;

EventResult
ConfigModel::HandleEvent(const Event &e) {
    if (e.type == UpdateBaseEvent) {
        base = static_cast<NumberBase>(e.data);
        return Consumed;
    } else if (e.type == UpdateWidthEvent) {
        width = static_cast<NumberWidth>(e.data);
        return Consumed;
    } else if (e.type == UpdateSignEvent) {
        sign = static_cast<NumberSign>(e.data);
        return Consumed;
    } else if (e.type == SwitchShiftModeEvent) {
        Direction dir = static_cast<Direction>(e.data);
        if (dir == DirLeft) {
            shiftMode = static_cast<NumberShiftMode>(
                (shiftMode - 1 + NumberShiftMode::MAX_SHIFT_MODE_COUNT) %
                NumberShiftMode::MAX_SHIFT_MODE_COUNT);
        } else if (dir == DirRight) {
            shiftMode = static_cast<NumberShiftMode>(
                (shiftMode + 1) % NumberShiftMode::MAX_SHIFT_MODE_COUNT);
        } else {
            // Should never reach here
            return Failed;
        }
        debugf("Shift mode changed to %d\n", shiftMode);
        return Consumed;
    } else {
        return Skipped;
    }

    return Skipped;
}
