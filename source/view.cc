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
    Area6x8 area(viewArea);
    Glyph demoGlyphs[] = {
        Glyph(Font6x8LBrac),    // (
        Glyph(Font6x8One),      // 1
        Glyph(FontEmpty),       //
        Glyph(Font6x8Multiply), // x
        Glyph(FontEmpty),       //
        Glyph(Font6x8Two),      // 2
        Glyph(FontEmpty),       //
        Glyph(Font6x8Divide),   // /
        Glyph(FontEmpty),       //
        Glyph(Font6x8C),        // C
        Glyph(Font6x8E),        // E
        Glyph(Font6x8RBrac),    // )
        Glyph(FontEmpty),       //
        Glyph(Font6x8And),      // &
        Glyph(FontEmpty),       //
        Glyph(Font6x8A),        // A
        Glyph(Font6x8F),        // F
        Glyph(FontEmpty),       //
        Glyph(Font6x8RShift),   // >>
        Glyph(FontEmpty),       //
        Glyph(Font6x8C),        // C
        Glyph(Font6x8C),        // C
        Glyph(FontEmpty),       //
        Glyph(Font6x8LShift),   // <<
        Glyph(FontEmpty),       //
        Glyph(Font6x8Two),      // 2
        Glyph(Font6x8Five),     // 5
        Glyph(Font6x8Six),      // 6
        Glyph(Font6x8Zero),     // 0
        Glyph(FontEmpty),       //
        Glyph(Font6x8Or),       // |
        Glyph(FontEmpty),       //
        Glyph(Font6x8Zero),     // 0
        Glyph(Font6x8Seven),    // 7
        Glyph(Font6x8Two),      // 2
        Glyph(Font6x8One),      // 1
        Glyph(FontEmpty),       //
        Glyph(Font6x8Equal),    // =
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    for (int16_t i = area.y; i < area.y + area.h; i++) {
        display.PrintLine<CharWidth>(demoGlyphs, glyphCount,
                                     area.w - glyphCount, i);
    }
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

    Area8x8 area(viewArea);

    debugf("area: x=%d, y=%d, w=%d, h=%d\n", area.x, area.y, area.w, area.h);
    // TODO render value to glyphs
    Glyph demoGlyphs[] = {
        Glyph(Font8x8A),     // A
        Glyph(Font8x8B),     // B
        Glyph(Font8x8C),     // C
        Glyph(Font8x8D),     // D
        Glyph(Font8x8E),     // E
        Glyph(Font8x8F),     // F
        Glyph(Font8x8One),   // 1
        Glyph(Font8x8Two),   // 2
        Glyph(Font8x8Three), // 3
        Glyph(Font8x8Comma), // ,
        Glyph(Font8x8Four),  // 4
        Glyph(Font8x8Five),  // 5
        Glyph(Font8x8Six),   // 6
        Glyph(Font8x8Seven), // 7
        Glyph(Font8x8Eight), // 8
        Glyph(Font8x8Nine),  // 9
        Glyph(Font8x8Zero),  // 0
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    for (int16_t i = area.y; i < area.y + area.h; i++) {
        display.PrintLine<CharWidth>(demoGlyphs, glyphCount,
                                     area.w - glyphCount, i);
    }
}

template <NumberBase base>
EventResult
TranscodeView<base>::handleValueChanged(void) {
    BasicView<TranscodeView<base>, MainDisplay>::markDirty();
    debugf("TranscodeView(%d) refreshed\n", static_cast<int>(base));
    // TODO render transcoded value to glyphs
    return Skipped;
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
