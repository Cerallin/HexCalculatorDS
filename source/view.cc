/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "view.h"
#include "config.h"
#include "subscreenArea.h"

using namespace HexCalc;

InputView::InputView(SubDisplay &display, ViewModel &vm)
    : SubView(display), vm(vm), handler(vm.Cmds()) {
    // Ordered by button position, left to right then top to bottom
    auto buttonAnd = handler.RegisterButton(
        Area(AREA_0_X, AREA_0_Y, AREA_0_W, AREA_0_H), ButtonType::ButtonAnd);
    auto buttonA = handler.RegisterButton(
        Area(AREA_1_X, AREA_1_Y, AREA_1_W, AREA_1_H), ButtonType::ButtonA);
    auto buttonB = handler.RegisterButton(
        Area(AREA_2_X, AREA_2_Y, AREA_2_W, AREA_2_H), ButtonType::ButtonB);
    auto buttonC = handler.RegisterButton(
        Area(AREA_3_X, AREA_3_Y, AREA_3_W, AREA_3_H), ButtonType::ButtonC);
    auto buttonD = handler.RegisterButton(
        Area(AREA_4_X, AREA_4_Y, AREA_4_W, AREA_4_H), ButtonType::ButtonD);
    auto buttonE = handler.RegisterButton(
        Area(AREA_5_X, AREA_5_Y, AREA_5_W, AREA_5_H), ButtonType::ButtonE);
    auto buttonF = handler.RegisterButton(
        Area(AREA_6_X, AREA_6_Y, AREA_6_W, AREA_6_H), ButtonType::ButtonF);
    auto buttonOr = handler.RegisterButton(
        Area(AREA_7_X, AREA_7_Y, AREA_7_W, AREA_7_H), ButtonType::ButtonOr);
    auto buttonLShift = handler.RegisterButton(
        Area(AREA_8_X, AREA_8_Y, AREA_8_W, AREA_8_H), ButtonType::ButtonLShift);
    auto buttonLBrac = handler.RegisterButton(
        Area(AREA_9_X, AREA_9_Y, AREA_9_W, AREA_9_H), ButtonType::ButtonLBrac);
    auto button7 = handler.RegisterButton(
        Area(AREA_10_X, AREA_10_Y, AREA_10_W, AREA_10_H), ButtonType::Button7);
    auto button4 = handler.RegisterButton(
        Area(AREA_11_X, AREA_11_Y, AREA_11_W, AREA_11_H), ButtonType::Button4);
    auto button1 = handler.RegisterButton(
        Area(AREA_12_X, AREA_12_Y, AREA_12_W, AREA_12_H), ButtonType::Button1);
    auto buttonNegate =
        handler.RegisterButton(Area(AREA_13_X, AREA_13_Y, AREA_13_W, AREA_13_H),
                               ButtonType::ButtonNegate);
    auto buttonModulo =
        handler.RegisterButton(Area(AREA_14_X, AREA_14_Y, AREA_14_W, AREA_14_H),
                               ButtonType::ButtonModulo);
    auto buttonRShift =
        handler.RegisterButton(Area(AREA_15_X, AREA_15_Y, AREA_15_W, AREA_15_H),
                               ButtonType::ButtonRShift);
    auto buttonRBrac =
        handler.RegisterButton(Area(AREA_16_X, AREA_16_Y, AREA_16_W, AREA_16_H),
                               ButtonType::ButtonRBrac);
    auto button8 = handler.RegisterButton(
        Area(AREA_17_X, AREA_17_Y, AREA_17_W, AREA_17_H), ButtonType::Button8);
    auto button5 = handler.RegisterButton(
        Area(AREA_18_X, AREA_18_Y, AREA_18_W, AREA_18_H), ButtonType::Button5);
    auto button2 = handler.RegisterButton(
        Area(AREA_19_X, AREA_19_Y, AREA_19_W, AREA_19_H), ButtonType::Button2);
    auto button0 = handler.RegisterButton(
        Area(AREA_20_X, AREA_20_Y, AREA_20_W, AREA_20_H), ButtonType::Button0);
    auto buttonClear =
        handler.RegisterButton(Area(AREA_21_X, AREA_21_Y, AREA_21_W, AREA_21_H),
                               ButtonType::ButtonClear);
    auto buttonDivide =
        handler.RegisterButton(Area(AREA_22_X, AREA_22_Y, AREA_22_W, AREA_22_H),
                               ButtonType::ButtonDivide);
    auto button9 = handler.RegisterButton(
        Area(AREA_23_X, AREA_23_Y, AREA_23_W, AREA_23_H), ButtonType::Button9);
    auto button6 = handler.RegisterButton(
        Area(AREA_24_X, AREA_24_Y, AREA_24_W, AREA_24_H), ButtonType::Button6);
    auto button3 = handler.RegisterButton(
        Area(AREA_25_X, AREA_25_Y, AREA_25_W, AREA_25_H), ButtonType::Button3);
    auto buttonBitwiseNot =
        handler.RegisterButton(Area(AREA_26_X, AREA_26_Y, AREA_26_W, AREA_26_H),
                               ButtonType::ButtonBitwiseNot);
    auto buttonBackspace =
        handler.RegisterButton(Area(AREA_27_X, AREA_27_Y, AREA_27_W, AREA_27_H),
                               ButtonType::ButtonBackspace);
    auto buttonMultiply =
        handler.RegisterButton(Area(AREA_28_X, AREA_28_Y, AREA_28_W, AREA_28_H),
                               ButtonType::ButtonMultiply);
    auto buttonMinus =
        handler.RegisterButton(Area(AREA_29_X, AREA_29_Y, AREA_29_W, AREA_29_H),
                               ButtonType::ButtonMinus);
    auto buttonPlus =
        handler.RegisterButton(Area(AREA_30_X, AREA_30_Y, AREA_30_W, AREA_30_H),
                               ButtonType::ButtonPlus);

    auto buttonEvaluate =
        handler.RegisterButton(Area(AREA_31_X, AREA_31_Y, AREA_31_W, AREA_31_H),
                               ButtonType::ButtonEvaluate);

    auto buttonWidthDrawer = handler.RegisterButton(
        Area(160 - 6, 0, 218 - 160, 25), ButtonType::ButtonChangeWidth);

    auto buttonSignDrawer = handler.RegisterButton(
        Area(224 - 6, 0, 250 - 224, 25), ButtonType::ButtonToggleSign);

    numberButtons[0x00] = button0;
    numberButtons[0x01] = button1;
    numberButtons[0x02] = button2;
    numberButtons[0x03] = button3;
    numberButtons[0x04] = button4;
    numberButtons[0x05] = button5;
    numberButtons[0x06] = button6;
    numberButtons[0x07] = button7;
    numberButtons[0x08] = button8;
    numberButtons[0x09] = button9;
    numberButtons[0x0A] = buttonA;
    numberButtons[0x0B] = buttonB;
    numberButtons[0x0C] = buttonC;
    numberButtons[0x0D] = buttonD;
    numberButtons[0x0E] = buttonE;
    numberButtons[0x0F] = buttonF;

    rightBracketButton = buttonRBrac;

    handleBaseChange();
}

EventResult
InputView::HandleEvent(const Event &e) {
    if (e.type == EventType::UpdateBaseEvent) {
        handleBaseChange();

        BasicView::markDirty();
        debugf("InputView invalidated by UpdateBaseEvent\n");

        return Consumed;
    } else if (e.type == EventType::UpdateSignEvent) {
        BasicView::markDirty();
        return Consumed;
    } else if (e.type == EventType::UpdateWidthEvent) {
        BasicView::markDirty();
        return Consumed;
    } else if (e.type == EventType::TouchScreenEvent) {
        Point touchPoint(e.data);

        debugf("Touch at (%d, %d)\n", touchPoint.x, touchPoint.y);

        handler.Handle(touchPoint);

        // FIXME wait for a while to avoid handling the same touch event
        // multiple times
        for (int i = 0; i < 4; i++) {
            swiWaitForVBlank();
        }

        // To update selected button state after handling touch input
        BasicView::markDirty();

        return Consumed;
    }

    return Skipped;
}

void
InputView::ForceUpdate(void) {
    debugf("InputView refreshed\n");
    // Update button states
    // minus 2 for width and sign drawers, which are handled separately
    for (size_t i = 0; i < handler.Size() - 2; i++) {
        const auto &button = handler.GetButton(i);
        if (button.Selected()) {
            debugf("Button %zu selected\n", i);
            display.SelectButton(i);
        } else if (button.Active()) {
            display.EnableButton(i);
        } else {
            display.DisableButton(i);
        }
    }
    // Update width and sign drawers
    auto width = config.Width();
    display.UpdateWidthDrawer(width);
    auto sign = config.Sign();
    display.UpdateSignDrawer(sign);
}

void
InputView::handleBaseChange(void) {
    auto base = config.Base();
    // Enable number buttons that are valid for the current base, and disable
    // them otherwise. For example, in hexadecimal mode, all number buttons are
    // enabled, while in decimal mode, only 0-9 buttons are enabled.
    for (size_t i = 0; i < 16; i++) {
        if (i < static_cast<int>(base)) {
            numberButtons[i]->Enable();
        } else {
            numberButtons[i]->Disable();
        }
    }
}

void
InputView::handleWidthChange(void) {
    auto width = config.Width();
    display.UpdateWidthDrawer(width);
}

void
InputView::handleSignChange(void) {
    auto sign = config.Sign();
    display.UpdateSignDrawer(sign);
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

    GlyphArray8x8<ConfigView::maxGlyphs> glyphs = getGlyphs();

    Point start(viewArea.x, viewArea.y);
    display.ClearLine(start, glyphs.CharWidth);
    display.PrintLine(glyphs, start);
}

GlyphArray8x8<ConfigView::maxGlyphs>
ConfigView::getGlyphs() {
    GlyphArray8x8<ConfigView::maxGlyphs> glyphs;
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
    if (e.type != EventType::FormulaUpdatedEvent) {
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

    clear();
    bake();

    // TODO pagination
    GlyphArray6x8<FormulaView::maxGlyphs> glyphs6x8(glyphs);

    auto skipGlyphs = area.w - glyphs.Size();
    Point start(viewArea.x + skipGlyphs * glyphs6x8.CharWidth, viewArea.y);
    display.PrintLine(glyphs6x8, start);
}

void
FormulaView::clear(void) {
    dmaFillWords(0, &glyphs, sizeof(glyphs));
}

void
FormulaView::bake(void) {
    // TODO bake formulaTree to glyphs

    // glyphs.Insert(Glyph(Font6x8LBrac));    // (
    // glyphs.Insert(Glyph(Font6x8One));      // 1
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Multiply)); // x
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Two));      // 2
    // glyphs.Insert(Glyph(Font6x8Five));     // 5
    // glyphs.Insert(Glyph(Font6x8Divide));   // /
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8C));        // C
    // glyphs.Insert(Glyph(Font6x8E));        // E
    // glyphs.Insert(Glyph(Font6x8RBrac));    // )
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8And));      // &
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8A));        // A
    // glyphs.Insert(Glyph(Font6x8F));        // F
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8RShift));   // >>
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8C));        // C
    // glyphs.Insert(Glyph(Font6x8C));        // C
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8LShift));   // <<
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Two));      // 2
    // glyphs.Insert(Glyph(Font6x8Five));     // 5
    // glyphs.Insert(Glyph(Font6x8Six));      // 6
    // glyphs.Insert(Glyph(Font6x8Zero));     // 0
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Or));       // |
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Zero));     // 0
    // glyphs.Insert(Glyph(Font6x8Seven));    // 7
    // glyphs.Insert(Glyph(Font6x8Two));      // 2
    // glyphs.Insert(Glyph(Font6x8One));      // 1
    // glyphs.Insert(Glyph(FontEmpty));       //
    // glyphs.Insert(Glyph(Font6x8Equal));    // =
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
        GlyphArray8x8<maxDigits> glyphs(digits, true);
        display.ClearLine(startLeft, glyphs.CharWidth);
        display.PrintLine(glyphs, startLeft);
    } else { // align right
        GlyphArray8x8<maxDigits> glyphs(digits);
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
    auto digits = number.Transcode<base, MaxDigitsForType<base>()>();
    using GlyphArrayType = std::conditional_t<
        (base == Hexadecimal), HexGlyphArray6x8,
        std::conditional_t<(base == Decimal), DecGlyphArray6x8,
                           std::conditional_t<(base == Octal), OctGlyphArray6x8,
                                              BinGlyphArray6x8>>>;
    GlyphArray6x8<MaxDigitsForType<base>()> glyphArray(digits, false);
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
        GlyphArray6x8<Number::MaxBinDigits> glyphArray(digits, false);
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
