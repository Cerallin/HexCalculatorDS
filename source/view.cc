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

void
HexCalc::InputView::ForceUpdate(void) {
    debugf("InputView refreshed\n");
}

EventResult
FormulaView::HandleEvent(const Event &e) {
    if (e.type != EventType::FormulaChangedEvent) {
        return Skipped;
    }

    BasicView<FormulaView, MainDisplay>::markDirty();
    // TODO render formulaTree to glyphs
    debugf("FormulaView invalidated\n");
    return Consumed;
}

void
FormulaView::ForceUpdate(void) {
    debugf("FormulaView refreshed\n");
}

EventResult
ValueView::HandleEvent(const Event &e) {
    if (e.type != EventType::ValueChangedEvent) {
        return Skipped;
    }

    BasicView<ValueView, MainDisplay>::markDirty();
    auto value = model.Value();
    debugf("ValueView updated: %08llx\n",
           static_cast<unsigned long long>(value));
    return Consumed;
}

void
ValueView::ForceUpdate(void) {
    debugf("ValueView refreshed\n");
    debugf("viewArea: x=%d, y=%d, w=%d, h=%d\n", viewArea.x, viewArea.y,
           viewArea.w, viewArea.h);
    Area6x8 area = viewArea;

    debugf("area: x=%d, y=%d, w=%d, h=%d\n", area.x, area.y, area.w, area.h);
    // TODO render value to glyphs
    for (int16_t i = area.x; i < area.x + area.w; i++) {
        for (int16_t j = area.y; j < area.y + area.h; j++) {
            // placeholder
            display.PutGlyph(i * 6, j * 8, Glyph(Font6x8Zero + i % 16));
        }
    }
}

template <NumberBase base>
EventResult
TranscodeView<base>::handleValueChanged(void) {
    BasicView<TranscodeView<base>, MainDisplay>::markDirty();
    debugf("TranscodeView(%d) refreshed\n", static_cast<int>(base));
    return Consumed;
}

template <>
void
TranscodeView<Hexadecimal>::ForceUpdate(void) {
    debugf("HexView refreshed\n");
}

template <>
void
TranscodeView<Decimal>::ForceUpdate(void) {
    debugf("DecView refreshed\n");
}

template <>
void
TranscodeView<Octal>::ForceUpdate(void) {
    debugf("OctView refreshed\n");
}

template <>
void
TranscodeView<Binary>::ForceUpdate(void) {
    debugf("BinView refreshed\n");
}

template class TranscodeView<Hexadecimal>;
template class TranscodeView<Decimal>;
template class TranscodeView<Octal>;
template class TranscodeView<Binary>;
