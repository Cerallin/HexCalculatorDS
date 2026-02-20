/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "view.h"
#include "config.h"

using namespace HexCalc;

EventResult
InputView::HandleEvent(const Event &e) {
    return Skipped;
}

void
InputView::ForceUpdate(void) {
    debugf("InputView refreshed\n");
}

EventResult
ConfigView::HandleEvent(const Event &e) {
    if (e.type == EventType::ClearEvent) {
        BasicView::markDirty();
        debugf("ConfigView cleared\n");
        return Consumed;
    } else if ((e.type != EventType::UpdateBaseEvent) &&
               (e.type != EventType::UpdateSignEvent) &&
               (e.type != EventType::UpdateWidthEvent)) {
        return Skipped;
    }

    BasicView::markDirty();
    // TODO render config to glyphs
    debugf("ConfigView invalidated\n");

    return Consumed;
}

void
ConfigView::ForceUpdate(void) {
    debugf("ConfigView refreshed\n");
    Area8x8 area(viewArea);
    // UINT64
    Glyph demoGlyphs[] = {
        Glyph(FontColoredU), Glyph(FontColoredI),   Glyph(FontColoredN),
        Glyph(FontColoredT), Glyph(FontColoredSix), Glyph(FontColoredFour),
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    Point start(viewArea.x, viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, 0, start);
}

EventResult
FormulaView::HandleEvent(const Event &e) {
    if (e.type != EventType::FormulaChangedEvent) {
        return Skipped;
    }

    BasicView::markDirty();
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
    Point start(viewArea.x, viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, area.w - glyphCount,
                                 start);
}

EventResult
ValueView::HandleEvent(const Event &e) {
    if (e.type != EventType::ValueChangedEvent) {
        return Skipped;
    }

    BasicView::markDirty();
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
    Point start(viewArea.x, viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, area.w - glyphCount,
                                 start);
}

template <NumberBase base>
EventResult
TranscodeView<base>::handleBaseChanged(void) {
    if (base != config.Base()) {
        return Skipped;
    }

    auto &viewArea = this->viewArea;
    BasicView<TranscodeView<base>, MainDisplay>::markDirty();
    debugf("TranscodeView(%d) base changed\n", static_cast<int>(base));

    // Clear indicator Area
    constexpr size_t indicatorAreaY = 8 * MainDisplay::TileHeight;
    constexpr size_t indicatorAreaHeight = 15;
    for (size_t j = 0; j < indicatorAreaHeight; j++) {
        this->display.template PutTile(
            barOffsetX, indicatorAreaY + (j * TileHeight), FontEmpty);
    }

    // Draw the indicator bar
    Area6x8 area(viewArea);
    auto middleH = (area.y + (area.h / 2)) * CharHeight;
    if constexpr (base == Binary) {
        middleH += CharHeight;
    }
    for (size_t i = 0; i < BarTileCount; i++) {
        this->display.template PutTile(barOffsetX + viewArea.x,
                                       middleH + i * TileHeight, BarTiles[i]);
    }

    return Consumed;
}

template <NumberBase base>
EventResult
TranscodeView<base>::handleValueChanged(void) {
    BasicView<TranscodeView<base>, MainDisplay>::markDirty();
    debugf("TranscodeView(%d) refreshed\n", static_cast<int>(base));
    // TODO render transcoded value to glyphs
    return Skipped;
}

template <NumberBase base>
void
HexCalc::TranscodeView<base>::printHeader(void) const {
    auto &viewArea = this->viewArea;
    Area6x8 area(viewArea);
    auto middleH = (area.y + area.h / 2) * CharHeight;
    if constexpr (base == Binary) {
        middleH += CharHeight;
    }
    Point start(viewArea.x, middleH);
    this->display.template PrintLine<CharWidth>(header, headerLength,
                                                headerSkip, start);
}

template <>
void
TranscodeView<Hexadecimal>::printNumber(void) const {
    // CEAD BEAF 0D00 0721
    Glyph demoGlyphs[] = {
        Glyph(Font6x8C),     // C
        Glyph(Font6x8E),     // E
        Glyph(Font6x8A),     // A
        Glyph(Font6x8D),     // D
        Glyph(FontEmpty),    //
        Glyph(Font6x8B),     // B
        Glyph(Font6x8E),     // E
        Glyph(Font6x8A),     // A
        Glyph(Font6x8F),     // F
        Glyph(FontEmpty),    //
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8D),     // D
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8Zero),  // 0
        Glyph(FontEmpty),    //
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Two),   // 2
        Glyph(Font6x8One),   // 1
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    Point start(viewArea.x +
                    (headerSkip + headerLength + numberGap) * CharWidth,
                viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, 0, start);
}

template <>
void
TranscodeView<Decimal>::printNumber(void) const {

    // -3,553,974,871,878,793,439
    Glyph demoGlyphs[] = {
        Glyph(Font6x8Minus), // -
        Glyph(Font6x8Three), // 3
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Three), // 3
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Nine),  // 9
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Eight), // 8
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8One),   // 1
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Eight), // 8
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Eight), // 8
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Nine),  // 9
        Glyph(Font6x8Three), // 3
        Glyph(Font6x8Comma), // ,
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8Three), // 3
        Glyph(Font6x8Nine),  // 9
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    Point start(viewArea.x +
                    (headerSkip + headerLength + numberGap) * CharWidth,
                viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, 0, start);
}

template <>
void
TranscodeView<Octal>::printNumber(void) const {
    // 1 472 555 752 741 500 003 441
    Glyph demoGlyphs[] = {
        Glyph(Font6x8One),   // 1
        Glyph(FontEmpty),    //
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Two),   // 2
        Glyph(FontEmpty),    //
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Five),  // 5
        Glyph(FontEmpty),    //
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Two),   // 2
        Glyph(FontEmpty),    //
        Glyph(Font6x8Seven), // 7
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8One),   // 1
        Glyph(FontEmpty),    //
        Glyph(Font6x8Five),  // 5
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8Zero),  // 0
        Glyph(FontEmpty),    //
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8Zero),  // 0
        Glyph(Font6x8Three), // 3
        Glyph(FontEmpty),    //
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8Four),  // 4
        Glyph(Font6x8One),   // 1
    };
    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    Point start(viewArea.x +
                    (headerSkip + headerLength + numberGap) * CharWidth,
                viewArea.y);
    display.PrintLine<CharWidth>(demoGlyphs, glyphCount, 0, start);
}

template <>
void
TranscodeView<Binary>::printNumber(void) const {
    // 1001 1010 1000 0010
    Glyph demoGlyphs[] = {
        Glyph(Font6x8One),  // 1
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8One),  // 1
        Glyph(FontEmpty),   //
        Glyph(Font6x8One),  // 1
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8One),  // 1
        Glyph(Font6x8Zero), // 0
        Glyph(FontEmpty),   //
        Glyph(Font6x8One),  // 1
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8Zero), // 0
        Glyph(FontEmpty),   //
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8Zero), // 0
        Glyph(Font6x8One),  // 1
        Glyph(Font6x8Zero), // 0
    };

    constexpr size_t glyphCount = sizeof(demoGlyphs) / sizeof(Glyph);
    for (int i = 0; i < 4; i++) {
        Point start(viewArea.x +
                        (headerSkip + headerLength + numberGap) * CharWidth,
                    viewArea.y + i * 2 * CharHeight);
        display.PrintLine<CharWidth>(demoGlyphs, glyphCount, 0, start);
    }
}

template <>
void
TranscodeView<Hexadecimal>::ForceUpdate(void) {
    debugf("HexView refreshed\n");
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Decimal>::ForceUpdate(void) {
    debugf("DecView refreshed\n");
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Octal>::ForceUpdate(void) {
    debugf("OctView refreshed\n");
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Binary>::ForceUpdate(void) {
    debugf("BinView refreshed\n");
    printHeader();
    printNumber();
}

template class TranscodeView<Hexadecimal>;
template class TranscodeView<Decimal>;
template class TranscodeView<Octal>;
template class TranscodeView<Binary>;
