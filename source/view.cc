/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "view.h"

using namespace HexCalc;

HandleEventResult
InputView::HandleEvent(const Event &e) {
    HandleEventResult res = Ignored;
    if (e.type == EventType::TouchScreenEvent) {
        Point touchPoint(e.data);
        // TODO handle touch event
        debugf("TouchScreenEvent: x=%d, y=%d\n", touchPoint.x, touchPoint.y);
        res = Handled;
    }
    return res;
}
