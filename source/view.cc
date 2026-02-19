/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "view.h"

using namespace HexCalc;

EventResult
InputView::HandleEvent(const Event &e) {
    EventResult res = Skipped;
    if (e.type == EventType::TouchScreenEvent) {
        Point touchPoint(e.data);
        // TODO handle touch event
        debugf("TouchScreenEvent: x=%d, y=%d\n", touchPoint.x, touchPoint.y);
        res = Consumed;
    }
    return res;
}

EventResult
FormulaView::HandleEvent(const Event &e) {
    if (e.type != EventType::FormulaChangedEvent) {
        return Skipped;
    }

    MarkDirty();
    // TODO render formulaTree to glyphs
    debugf("FormulaView invalidated\n");
    return Consumed;
}

EventResult
ValueView::HandleEvent(const Event &e) {
    if (e.type != EventType::ValueChangedEvent) {
        return Skipped;
    }

    MarkDirty();
    auto value = model.Value();
    debugf("ValueView updated: %08llx\n",
           static_cast<unsigned long long>(value));
    return Consumed;
}

template <NumberBase base>
EventResult
TranscodeView<base>::HandleEvent(const Event &e) {
    if (e.type != EventType::ValueChangedEvent) {
        return Skipped;
    }

    return HandleValueChanged();
}

template <NumberBase base>
EventResult
TranscodeView<base>::HandleValueChanged(void) {
    MarkDirty();
    debugf("TranscodeView(%d) refreshed\n", static_cast<int>(base));
    return Consumed;
}

template class TranscodeView<Hexadecimal>;
template class TranscodeView<Decimal>;
template class TranscodeView<Octal>;
template class TranscodeView<Binary>;
