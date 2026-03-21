/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "view.h"
#include "subscreenArea.h"

using namespace HexCalc;

InputView::InputView(SubDisplay &display, ViewModel &vm)
    : SubView(display), vm(vm), handler(vm.Cmds()), leftBracketCount(0) {
    // Ordered by button position, left to right then top to bottom
    HEXCALC_GCC_UNUSED auto buttonAnd =
        handler.RegisterButton(Area(AREA_0_X, AREA_0_Y, AREA_0_W, AREA_0_H),
                               ButtonType::ButtonAnd, 0, 0);
    HEXCALC_GCC_UNUSED auto buttonA =
        handler.RegisterButton(Area(AREA_1_X, AREA_1_Y, AREA_1_W, AREA_1_H),
                               ButtonType::ButtonA, 0, 1);
    HEXCALC_GCC_UNUSED auto buttonB =
        handler.RegisterButton(Area(AREA_2_X, AREA_2_Y, AREA_2_W, AREA_2_H),
                               ButtonType::ButtonB, 0, 2);
    HEXCALC_GCC_UNUSED auto buttonC =
        handler.RegisterButton(Area(AREA_3_X, AREA_3_Y, AREA_3_W, AREA_3_H),
                               ButtonType::ButtonC, 0, 3);
    HEXCALC_GCC_UNUSED auto buttonD =
        handler.RegisterButton(Area(AREA_4_X, AREA_4_Y, AREA_4_W, AREA_4_H),
                               ButtonType::ButtonD, 0, 4);
    HEXCALC_GCC_UNUSED auto buttonE =
        handler.RegisterButton(Area(AREA_5_X, AREA_5_Y, AREA_5_W, AREA_5_H),
                               ButtonType::ButtonE, 0, 5);
    HEXCALC_GCC_UNUSED auto buttonF =
        handler.RegisterButton(Area(AREA_6_X, AREA_6_Y, AREA_6_W, AREA_6_H),
                               ButtonType::ButtonF, 0, 6);
    HEXCALC_GCC_UNUSED auto buttonOr =
        handler.RegisterButton(Area(AREA_7_X, AREA_7_Y, AREA_7_W, AREA_7_H),
                               ButtonType::ButtonOr, 1, 0);
    HEXCALC_GCC_UNUSED auto buttonLShift =
        handler.RegisterButton(Area(AREA_8_X, AREA_8_Y, AREA_8_W, AREA_8_H),
                               ButtonType::ButtonLShift, 1, 1);
    HEXCALC_GCC_UNUSED auto buttonLBrac =
        handler.RegisterButton(Area(AREA_9_X, AREA_9_Y, AREA_9_W, AREA_9_H),
                               ButtonType::ButtonLBrac, 1, 2);
    HEXCALC_GCC_UNUSED auto button7 =
        handler.RegisterButton(Area(AREA_10_X, AREA_10_Y, AREA_10_W, AREA_10_H),
                               ButtonType::Button7, 1, 3);
    HEXCALC_GCC_UNUSED auto button4 =
        handler.RegisterButton(Area(AREA_11_X, AREA_11_Y, AREA_11_W, AREA_11_H),
                               ButtonType::Button4, 1, 4);
    HEXCALC_GCC_UNUSED auto button1 =
        handler.RegisterButton(Area(AREA_12_X, AREA_12_Y, AREA_12_W, AREA_12_H),
                               ButtonType::Button1, 1, 5);
    HEXCALC_GCC_UNUSED auto buttonNegate =
        handler.RegisterButton(Area(AREA_13_X, AREA_13_Y, AREA_13_W, AREA_13_H),
                               ButtonType::ButtonNegate, 1, 6);
    HEXCALC_GCC_UNUSED auto buttonModulo =
        handler.RegisterButton(Area(AREA_14_X, AREA_14_Y, AREA_14_W, AREA_14_H),
                               ButtonType::ButtonModulo, 2, 0);
    HEXCALC_GCC_UNUSED auto buttonRShift =
        handler.RegisterButton(Area(AREA_15_X, AREA_15_Y, AREA_15_W, AREA_15_H),
                               ButtonType::ButtonRShift, 2, 1);
    HEXCALC_GCC_UNUSED auto buttonRBrac =
        handler.RegisterButton(Area(AREA_16_X, AREA_16_Y, AREA_16_W, AREA_16_H),
                               ButtonType::ButtonRBrac, 2, 2);
    HEXCALC_GCC_UNUSED auto button8 =
        handler.RegisterButton(Area(AREA_17_X, AREA_17_Y, AREA_17_W, AREA_17_H),
                               ButtonType::Button8, 2, 3);
    HEXCALC_GCC_UNUSED auto button5 =
        handler.RegisterButton(Area(AREA_18_X, AREA_18_Y, AREA_18_W, AREA_18_H),
                               ButtonType::Button5, 2, 4);
    HEXCALC_GCC_UNUSED auto button2 =
        handler.RegisterButton(Area(AREA_19_X, AREA_19_Y, AREA_19_W, AREA_19_H),
                               ButtonType::Button2, 2, 5);
    HEXCALC_GCC_UNUSED auto button0 =
        handler.RegisterButton(Area(AREA_20_X, AREA_20_Y, AREA_20_W, AREA_20_H),
                               ButtonType::Button0, 2, 6);
    HEXCALC_GCC_UNUSED auto buttonClear =
        handler.RegisterButton(Area(AREA_21_X, AREA_21_Y, AREA_21_W, AREA_21_H),
                               ButtonType::ButtonClear, 3, 1);
    HEXCALC_GCC_UNUSED auto buttonDivide =
        handler.RegisterButton(Area(AREA_22_X, AREA_22_Y, AREA_22_W, AREA_22_H),
                               ButtonType::ButtonDivide, 3, 2);
    HEXCALC_GCC_UNUSED auto button9 =
        handler.RegisterButton(Area(AREA_23_X, AREA_23_Y, AREA_23_W, AREA_23_H),
                               ButtonType::Button9, 3, 3);
    HEXCALC_GCC_UNUSED auto button6 =
        handler.RegisterButton(Area(AREA_24_X, AREA_24_Y, AREA_24_W, AREA_24_H),
                               ButtonType::Button6, 3, 4);
    HEXCALC_GCC_UNUSED auto button3 =
        handler.RegisterButton(Area(AREA_25_X, AREA_25_Y, AREA_25_W, AREA_25_H),
                               ButtonType::Button3, 3, 5);
    HEXCALC_GCC_UNUSED auto buttonBitwiseNot =
        handler.RegisterButton(Area(AREA_26_X, AREA_26_Y, AREA_26_W, AREA_26_H),
                               ButtonType::ButtonBitwiseNot, 3, 6);
    HEXCALC_GCC_UNUSED auto buttonBackspace =
        handler.RegisterButton(Area(AREA_27_X, AREA_27_Y, AREA_27_W, AREA_27_H),
                               ButtonType::ButtonBackspace, 4, 1);
    HEXCALC_GCC_UNUSED auto buttonMultiply =
        handler.RegisterButton(Area(AREA_28_X, AREA_28_Y, AREA_28_W, AREA_28_H),
                               ButtonType::ButtonMultiply, 4, 2);
    HEXCALC_GCC_UNUSED auto buttonMinus =
        handler.RegisterButton(Area(AREA_29_X, AREA_29_Y, AREA_29_W, AREA_29_H),
                               ButtonType::ButtonMinus, 4, 3);
    HEXCALC_GCC_UNUSED auto buttonPlus =
        handler.RegisterButton(Area(AREA_30_X, AREA_30_Y, AREA_30_W, AREA_30_H),
                               ButtonType::ButtonPlus, 4, 4);

    HEXCALC_GCC_UNUSED auto buttonEvaluate =
        handler.RegisterButton(Area(AREA_31_X, AREA_31_Y, AREA_31_W, AREA_31_H),
                               ButtonType::ButtonEvaluate, 4, 6);

    HEXCALC_GCC_UNUSED auto buttonWidthDrawer = handler.RegisterButton(
        Area(160 - 6, 0, 218 - 160, 25), ButtonType::ButtonChangeWidth, 3, 0);

    HEXCALC_GCC_UNUSED auto buttonSignDrawer = handler.RegisterButton(
        Area(224 - 6, 0, 250 - 224, 25), ButtonType::ButtonToggleSign, 4, 0);

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

    // Initialize left bracket sprites
    leftBracketSprites[0] = display.AddSprite(Point(82, 73));
    leftBracketSprites[1] = display.AddSprite(Point(82 + 5, 73));

    rightBracketButton = buttonRBrac;
    // Initially disable right bracket button since there is no left bracket in
    // the formula
    rightBracketButton->Disable();

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
    } else if (e.type == EventType::MoveFocusEvent) {
        Point pos{0, 0};
        const auto *focused = handler.FocusedButton();
        if (focused != nullptr) {
            pos = focused->Position();
        }
        auto dir = static_cast<Direction>(e.data);
        auto &button = getFocus(pos, dir);
        handler.ChangeFocus(&button);

        BasicView::markDirty();
        return Consumed;
    } else if (e.type == EventType::PreviousTouchEvent) {
        handler.PressFocus();
        return Consumed;
    } else if (e.type == EventType::TouchScreenEvent) {
        Point touchPoint(e.data);

        debugf("Touch at (%d, %d)\n", touchPoint.x, touchPoint.y);

        handler.Handle(touchPoint);

        // To update selected button state after handling touch input
        BasicView::markDirty();

        return Consumed;
    } else if (e.type == EventType::OperatorAcceptEvent) {
        auto newLeftBracketCount = vm.GetLeftBracketCount();
        if (newLeftBracketCount != leftBracketCount) {
            updateLBrackCount(newLeftBracketCount);
            // Reset right bracket button status
            BasicView::markDirty();

            return Consumed;
        }
        return Skipped;
    } else if (e.type == EventType::ClearEvent) {
        updateLBrackCount(0);
        // Reset right bracket button status
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
    auto width = vm.GetNumberWidth();
    display.UpdateWidthDrawer(width);
    auto sign = vm.GetNumberSign();
    display.UpdateSignDrawer(sign);
}

void
InputView::handleBaseChange(void) {
    auto base = vm.GetNumberBase();
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
InputView::updateLBrackCount(int count) {
    leftBracketCount = count;
    if (leftBracketCount < 0) {
        leftBracketCount = 0;
    }
    if (leftBracketCount > 99) {
        leftBracketCount = 99;
    }

    if (leftBracketCount <= 0) {
        rightBracketButton->Disable();
    } else {
        rightBracketButton->Enable();
    }

    auto num1 = leftBracketCount / 10;
    auto num2 = leftBracketCount % 10;

    if (leftBracketCount != 0) {
        if (num1 == 0) {
            leftBracketSprites[0]->SetTileOffset(num2 + 1);
            leftBracketSprites[1]->SetTileOffset(0);
        } else {
            leftBracketSprites[0]->SetTileOffset(num1 + 1);
            leftBracketSprites[1]->SetTileOffset(num2 + 1);
        }
    } else {
        leftBracketSprites[0]->SetTileOffset(0);
        leftBracketSprites[1]->SetTileOffset(0);
    }
}

TouchButton &
InputView::getFocus(Point position, Direction dir) {
    Point nextPos = handler.NavigateFocus(position, dir);

    auto buttonPtr = handler.GetMatrix(nextPos);
    assert(buttonPtr != nullptr);

    if (!buttonPtr->Active()) {
        return getFocus(nextPos, dir);
    }

    return *buttonPtr;
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
    auto width = vm.GetNumberWidth();
    auto sign = vm.GetNumberSign();

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
    debugf("FormulaView invalidated\n");

    return Consumed;
}

void
FormulaView::ForceUpdate(void) {
    debugf("FormulaView refreshed\n");
    Area6x8 area(viewArea);

    // Clear all glyphs
    clear();

    const auto &paginator = vm.GetFormulaPaginator();
    const auto &glyphs = paginator.Glyphs();

    // Draw current page of formula glyphs, aligned to the right
    auto skipGlyphs = area.w - glyphs.Size();
    Point start(viewArea.x + skipGlyphs * glyphs.CharWidth, viewArea.y);
    display.PrintLine(glyphs, start);
    // Draw pagination indicators
    // <- indicates there are more glyphs on the left (next page)
    // 2 is offset
    auto nextX = viewArea.x + 2;
    auto nextY = viewArea.y;
    if (paginator.HasNextPage()) {
        display.PrintGlyph(nextX, nextY, Glyph(Font6x8LArrow));
    } else {
        display.PrintGlyph(nextX, nextY, Glyph(FontEmpty));
    }
    // -> indicates there are more glyphs on the right (previous page)
    auto prevX = viewArea.x + (area.w * glyphs.CharWidth);
    auto prevY = viewArea.y;
    if (paginator.HasPreviousPage()) {
        display.PrintGlyph(prevX, prevY, Glyph(Font6x8RArrow));
    } else {
        display.PrintGlyph(prevX, prevY, Glyph(FontEmpty));
    }
}

void
FormulaView::clear(void) {
    Point start(viewArea.x, viewArea.y);
    display.ClearLine(start, CharWidth);
}

static constexpr auto
makeErrorGlyphs(FormulaEvaluateResult err) {
    assert(err != EvalSuccess);

    // FIXME magic number
    GlyphArray8x8<28> glyphs;

    switch (err) {
    case DivideByZero:
        // divide by zero
        glyphs.Insert(Glyph(FontErrorD));
        glyphs.Insert(Glyph(FontErrorI));
        glyphs.Insert(Glyph(FontErrorV));
        glyphs.Insert(Glyph(FontErrorI));
        glyphs.Insert(Glyph(FontErrorD));
        glyphs.Insert(Glyph(FontErrorE));
        glyphs.Insert(Glyph(FontEmpty));
        glyphs.Insert(Glyph(FontErrorB));
        glyphs.Insert(Glyph(FontErrorY));
        glyphs.Insert(Glyph(FontEmpty));
        glyphs.Insert(Glyph(FontErrorZ));
        glyphs.Insert(Glyph(FontErrorE));
        glyphs.Insert(Glyph(FontErrorR));
        glyphs.Insert(Glyph(FontErrorO));
        break;
    default:
        glyphs.Insert(Glyph(FontErrorE));
        glyphs.Insert(Glyph(FontErrorR));
        glyphs.Insert(Glyph(FontErrorR));
        glyphs.Insert(Glyph(FontErrorO));
        glyphs.Insert(Glyph(FontErrorR));
        break;
    }

    return glyphs;
}

EventResult
ValueView::HandleEvent(const Event &e) {
    if (e.type == EventType::ValueChangedEvent) {
        BasicView::markDirty();
        debugf("ValueView updated\n");
        return Consumed;
    } else if (e.type == EventType::UpdateBaseEvent) {
        BasicView::markDirty();
        debugf("ValueView base changed\n");
        return Consumed;
    } else if (e.type == EventType::EvaluateErrorEvent) {
        BasicView::markDirty();
        debugf("ValueView evaluation error: %d\n", e.data);
        lastEvaluateResult = static_cast<FormulaEvaluateResult>(e.data);
        return Consumed;
    } else if (e.type == EventType::ClearEvent) {
        if (lastEvaluateResult != EvalSuccess) {
            BasicView::markDirty();
            debugf("ValueView cleared\n");
            lastEvaluateResult = EvalSuccess;
            return Consumed;
        }
        return Skipped;
    } else {
        return Skipped;
    }
}

void
ValueView::ForceUpdate(void) {
    debugf("ValueView refreshed\n");

    if (lastEvaluateResult == EvalSuccess) {
        auto base = vm.GetNumberBase();
        auto digits = vm.GetValueDigits<MaxDisplayDigits>(base);
        PrintFormattedGlyphs<8, 8>(base, digits, true);
    } else {
        auto glyphs = makeErrorGlyphs(DivideByZero);
        PrintFormattedGlyphs<8, 8>(glyphs, true);
    }
}

EventResult
IndicatorView::HandleEvent(const Event &e) {
    if (e.type == EventType::UpdateBaseEvent) {
        auto nextBase = vm.GetNumberBase();
        if (nextBase == currentBase) {
            return Skipped;
        }

        BasicView::markDirty();
        return Consumed;
    }

    return Skipped;
}

void
IndicatorView::ForceUpdate(void) {
    debugf("IndicatorView refreshed\n");

    auto x = viewArea.x;
    auto previousY = getIndicatorY(currentBase);

    for (size_t j = 0; j < BarTileCount; j++) {
        auto y = previousY + j * TileHeight;
        this->display.PutTile(x, y, FontEmpty);
    }

    auto nextBase = vm.GetNumberBase();
    auto indicatorY = getIndicatorY(nextBase);

    for (size_t j = 0; j < BarTileCount; j++) {
        auto y = indicatorY + j * TileHeight;
        this->display.PutTile(x, y, BarTiles[j]);
    }

    currentBase = nextBase;
}

int16_t
IndicatorView::getIndicatorY(NumberBase base) const {
    int16_t line = HexView::line;
    int16_t height = HexView::height;

    switch (base) {
    case Hexadecimal:
        line = HexView::line;
        height = HexView::height;
        break;
    case Decimal:
        line = DecView::line;
        height = DecView::height;
        break;
    case Octal:
        line = OctView::line;
        height = OctView::height;
        break;
    case Binary:
        line = BinView::line;
        height = BinView::height;
        break;
    default:
        break;
    }

    Area baseArea(0, line * HexView::TileHeight,
                  HexView::lineWidth * HexView::TileWidth,
                  height * HexView::TileHeight);
    Area6x8 area(baseArea);
    auto middleY = (area.y + (area.h / 2)) * HexView::CharHeight;
    if (base == Binary) {
        middleY += HexView::CharHeight;
    }
    return middleY;
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
    auto digits = vm.GetValueDigits<MaxDigitsForType<base>()>(base);
    auto glyphs = MakeFormattedGlyphArray<base, 6, 8>(digits, false);

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
    for (int i = 0; i < 4; i++) {
        auto digits = vm.GetValueDigitsPerByte<Number::MaxBinDigits>(i, Binary);
        auto glyphs = MakeFormattedGlyphArray<Binary, 6, 8>(digits, false);
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
