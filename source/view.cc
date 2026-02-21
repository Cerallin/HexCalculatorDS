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
    if (e.type != EventType::TouchScreenEvent) {
        return Skipped;
    }

    Point touchPoint(e.data);

    debugf("Touch at (%d, %d)\n", touchPoint.x, touchPoint.y);

    return Consumed;
}

void
InputView::ForceUpdate(void) {
    debugf("InputView refreshed\n");
}

EventResult
ConfigView::HandleEvent(const Event &e) {
    if ((e.type != EventType::UpdateBaseEvent) &&
        (e.type != EventType::UpdateSignEvent) &&
        (e.type != EventType::UpdateWidthEvent)) {
        return Skipped;
    }

    BasicView::markDirty();
    debugf("ConfigView invalidated\n");

    return Consumed;
}

void
ConfigView::ForceUpdate(void) {
    debugf("ConfigView refreshed\n");

    auto glyphArray = getGlyphs();
    GlyphArray8x8 glyphs(glyphArray);

    Point start(viewArea.x, viewArea.y);
    display.PrintLine(glyphs, start);
}

GlyphArray<ConfigView::maxGlyphs>
ConfigView::getGlyphs() {
    GlyphArray<ConfigView::maxGlyphs> glyphs;
    auto width = config.Width();
    auto sign = config.Sign();

    if (sign == Unsigned) {
        glyphs.Insert(Glyph(FontColoredU));
    }
    glyphs.Insert(Glyph(FontColoredI));
    glyphs.Insert(Glyph(FontColoredN));
    glyphs.Insert(Glyph(FontColoredT));
    switch (width) {
    case QWord:
        glyphs.Insert(Glyph(FontColoredSix));
        glyphs.Insert(Glyph(FontColoredFour));
        break;
    case DWord:
        glyphs.Insert(Glyph(FontColoredThree));
        glyphs.Insert(Glyph(FontColoredTwo));
        break;
    case Word:
        glyphs.Insert(Glyph(FontColoredOne));
        glyphs.Insert(Glyph(FontColoredSix));
        break;
    case Byte:
        glyphs.Insert(Glyph(FontColoredEight));
        break;
    default:
        break;
    }

    return glyphs;
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
    GlyphArray<40> demoGlyphs;
    demoGlyphs.Insert(Glyph(Font6x8LBrac));    // (
    demoGlyphs.Insert(Glyph(Font6x8One));      // 1
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Multiply)); // x
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Two));      // 2
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Divide));   // /
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8C));        // C
    demoGlyphs.Insert(Glyph(Font6x8E));        // E
    demoGlyphs.Insert(Glyph(Font6x8RBrac));    // )
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8And));      // &
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8A));        // A
    demoGlyphs.Insert(Glyph(Font6x8F));        // F
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8RShift));   // >>
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8C));        // C
    demoGlyphs.Insert(Glyph(Font6x8C));        // C
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8LShift));   // <<
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Two));      // 2
    demoGlyphs.Insert(Glyph(Font6x8Five));     // 5
    demoGlyphs.Insert(Glyph(Font6x8Six));      // 6
    demoGlyphs.Insert(Glyph(Font6x8Zero));     // 0
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Or));       // |
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Zero));     // 0
    demoGlyphs.Insert(Glyph(Font6x8Seven));    // 7
    demoGlyphs.Insert(Glyph(Font6x8Two));      // 2
    demoGlyphs.Insert(Glyph(Font6x8One));      // 1
    demoGlyphs.Insert(Glyph(FontEmpty));       //
    demoGlyphs.Insert(Glyph(Font6x8Equal));    // =

    GlyphArray6x8 glyphs(demoGlyphs);

    auto skipGlyphs = area.w - glyphs.Size();
    Point start(viewArea.x + skipGlyphs * glyphs.CharWidth, viewArea.y);
    display.PrintLine(glyphs, start);
}

EventResult
ValueView::HandleEvent(const Event &e) {
    if (e.type == EventType::ValueChangedEvent) {
        BasicView::markDirty();
        debugf("ValueView updated\n");
        auto value = vm.GetNumber();

        return Consumed;
    } else if (e.type == EventType::UpdateBaseEvent) {
        BasicView::markDirty();
        debugf("ValueView base changed\n");

        return Consumed;
    } else {
        return Skipped;
    }
}

void
ValueView::ForceUpdate(void) {
    debugf("ValueView refreshed\n");

    auto sign = config.Sign();
    auto base = config.Base();
    auto width = config.Width();

    constexpr auto maxDigits = Number::MaxDecDigits;

    DigitArray<maxDigits> digits;
    Number number(vm.GetNumber(), sign);

    if (base == Hexadecimal) {
        digits = number.Transcode<Hexadecimal, maxDigits>();
    } else if (base == Decimal) {
        digits = number.Transcode<Decimal, maxDigits>();
    } else if (base == Octal) {
        digits = number.Transcode<Octal, maxDigits>();
    } else if (base == Binary) {
        digits = number.Transcode<Binary, maxDigits>();
    } else {
        debugf("ValueView number: %llu\n",
               static_cast<unsigned long long>(
                   number.Transcode<Decimal, maxDigits>()[0]));
    }

    Point startLeft(viewArea.x, viewArea.y);
    if (MainView::viewAlign == AlignLeft) {
        GlyphArray8x8 glyphs(digits, true);
        display.ClearLine(startLeft, glyphs.CharWidth);
        display.PrintLine(glyphs, startLeft);
    } else { // align right
        GlyphArray8x8 glyphs(digits);
        Area8x8 area(viewArea);
        auto skipGlyphs = area.w - glyphs.Size();
        Point startRight(viewArea.x + (skipGlyphs * glyphs.CharWidth),
                         viewArea.y);
        display.ClearLine(startLeft, glyphs.CharWidth);
        display.PrintLine(glyphs, startRight);
    }
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
    // FIXME create a new view for the indicator bar
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
    printNumber();
    return Consumed;
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
    Point start(viewArea.x + headerSkip * GlyphArray6x8<0>::CharWidth, middleH);
    this->display.template PrintLine(header, start);
}

template <NumberBase base>
void
TranscodeView<base>::printNumber(void) const {
    Number number(vm.GetNumber(), config.Sign());
    auto digits = number.Transcode<base, Number::MaxDisplayDigits>();
    using GlyphArrayType = std::conditional_t<
        (base == Hexadecimal), HexGlyphArray6x8,
        std::conditional_t<(base == Decimal), DecGlyphArray6x8,
                           std::conditional_t<(base == Octal), OctGlyphArray6x8,
                                              BinGlyphArray6x8>>>;
    GlyphArray6x8 glyphArray(digits, false);
    GlyphArrayType glyphs(glyphArray);

    Point start(this->viewArea.x +
                    (headerSkip + header.Size() + numberGap) * CharWidth,
                this->viewArea.y);
    this->display.ClearLine(start, glyphs.CharWidth);
    this->display.PrintLine(glyphs, start);
}

template <>
void
TranscodeView<Binary>::printNumber(void) const {
    // TODO Adjust the height of TranscodeView dynamically to accommodate
    // different widths of number display
    auto width = config.Width();
    auto sign = config.Sign();
    for (int i = 0; i < 4; i++) {
        auto nByte =
            static_cast<uint16_t>((vm.GetNumber() >> (i * 16)) & 0xFFFF);
        debugf("TranscodeView(Binary) byte %d: %04x\n", i, nByte);
        Number number(nByte, sign);
        auto digits = number.Transcode<Binary, Number::MaxBinDigits>();
        GlyphArray6x8 glyphArray(digits, false);
        BinGlyphArray6x8 glyphs(glyphArray);
        Point start(this->viewArea.x +
                        (headerSkip + header.Size() + numberGap) * CharWidth,
                    this->viewArea.y + (3 - i) * lineHeight * CharHeight);
        this->display.PrintLine(glyphs, start);
    }
}

template <>
void
TranscodeView<Hexadecimal>::ForceUpdate(void) {
    debugf("HexView refreshed\n");
    handleBaseChanged();
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Decimal>::ForceUpdate(void) {
    debugf("DecView refreshed\n");
    handleBaseChanged();
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Octal>::ForceUpdate(void) {
    debugf("OctView refreshed\n");
    handleBaseChanged();
    printHeader();
    printNumber();
}

template <>
void
TranscodeView<Binary>::ForceUpdate(void) {
    debugf("BinView refreshed\n");
    handleBaseChanged();
    printHeader();
    printNumber();
}

template class TranscodeView<Hexadecimal>;
template class TranscodeView<Decimal>;
template class TranscodeView<Octal>;
template class TranscodeView<Binary>;
