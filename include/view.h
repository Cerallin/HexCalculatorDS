/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "display.h"
#include "event.h"
#include "format.h"
#include "input.h"
#include "viewmodel.h"

namespace HexCalc {

template <class Derived, typename DisplayType>
class BasicView {
  public:
    // Initially, the view needs to be rendered at least once.
    explicit BasicView(DisplayType &display) : display(display), dirty(true) {}

    EventResult
    HandleEvent(const Event &e) {
        return Skipped;
    }

    void
    Update(void) {
        if (dirty) {
            static_cast<Derived *>(this)->ForceUpdate();
        }
        dirty = false;
    }

  protected:
    DisplayType &display;

    void
    markDirty(void) {
        dirty = true;
    }

  private:
    bool dirty;
};

/**
 * @brief The alignment of the view text.
 *
 */
enum ViewAlign : uint8_t {
    AlignLeft,
    AlignRight,
};

/**
 * @brief Basic view on the main screen.
 *
 */
template <class Derived, ViewAlign Align>
class MainView : public BasicView<Derived, MainDisplay> {
  public:
    MainView(Area area, MainDisplay &display)
        : BasicView<Derived, MainDisplay>(display), viewArea(area) {}

    static constexpr auto viewAlign = Align;

    template <int W, int H, size_t N>
    void
    PrintFormattedGlyphs(NumberBase base, DigitArray<N> digits,
                         bool underline = false) const {
        Point start(viewArea.x, viewArea.y);
        constexpr bool alignLeft = (Derived::viewAlign == AlignLeft);
        VisitFormattedGlyphArray<W, H>(
            base, digits, alignLeft,
            [this, start, underline](const auto &formattedGlyphs) {
                return this->PrintFormattedGlyphs<W, H>(formattedGlyphs,
                                                        underline);
            });
    }

    template <int W, int H, size_t N>
    void
    PrintFormattedGlyphs(GlyphArray8x8<N> glyphs,
                         bool underline = false) const {
        Point start(viewArea.x, viewArea.y);
        constexpr bool alignLeft = (Derived::viewAlign == AlignLeft);
        if constexpr (alignLeft) {
            this->display.ClearLine(start, glyphs.CharWidth, underline);
            this->display.PrintLine(glyphs, start);
        } else {
            Area8x8 area(viewArea);
            auto skipGlyphs = area.w - glyphs.Size();
            Point glyphStart(viewArea.x + (skipGlyphs * glyphs.CharWidth),
                             viewArea.y);
            this->display.ClearLine(start, glyphs.CharWidth, underline);
            this->display.PrintLine(glyphs, glyphStart);
        }
    }

  protected:
    Area viewArea;
};

class ConfigView : public MainView<ConfigView, AlignLeft> {
  public:
    ConfigView(MainDisplay &display, ViewModel &vm)
        : MainView(Area(offsetX, line * MainDisplay::TileHeight,
                        lineWidth * MainDisplay::TileWidth,
                        height * MainDisplay::TileHeight),
                   display),
          vm(vm) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

    static constexpr int16_t offsetX = 12;

    static constexpr int16_t height = 2;
    static constexpr int16_t line = 0;
    static constexpr int16_t lineWidth = 30;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharWidth = 8;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharHeight = 8;

  private:
    static constexpr size_t maxGlyphs = 6;

    ViewModel &vm;

    GlyphArray8x8<maxGlyphs> getGlyphs();
};

class FormulaView : public MainView<FormulaView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    FormulaView(MainDisplay &display, ViewModel &vm)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          vm(vm), page(0) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

    static constexpr int16_t offsetX = 2;

    static constexpr int16_t height = 2;
    static constexpr int16_t line = ConfigView::height;
    static constexpr int16_t lineWidth = 30;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharWidth = 6;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharHeight = 8;

  private:
    ViewModel &vm;
    int page;

    /**
     * @brief Clear the formula glyphs and pagination indicators.
     *
     */
    void clear(void);
};

class ValueView : public MainView<ValueView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    ValueView(MainDisplay &display, ViewModel &vm)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          vm(vm), lastEvaluateResult(EvalSuccess) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

    static constexpr int16_t offsetX = 2;

    static constexpr int16_t height = 3;
    static constexpr int16_t line = 2 + FormulaView::height;
    static constexpr int16_t lineWidth = 30;
    static constexpr size_t MaxDisplayGlyphs = 28;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharWidth = 8;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharHeight = 8;

  private:
    ViewModel &vm;

    FormulaEvaluateResult lastEvaluateResult;
};

template <NumberBase Base>
class TranscodeView : public MainView<TranscodeView<Base>, AlignLeft> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    TranscodeView(MainDisplay &display, ViewModel &vm)
        : MainView<TranscodeView<Base>, AlignLeft>(
              //
              Area(0, line * TileHeight, lineWidth * TileWidth,
                   height * TileHeight),
              display),
          vm(vm), selected(false) {}

    EventResult
    HandleEvent(const Event &e) {
        if (e.type == EventType::ValueChangedEvent) {
            BasicView<TranscodeView<Base>, MainDisplay>::markDirty();
            debugf("TranscodeView(%d) refreshed\n", static_cast<int>(Base));
            printNumber();
            return Consumed;
        }

        return Skipped;
    }

    void ForceUpdate(void);

    // hex: 2, dec: 2, oct: 3, bin: 8
    static constexpr int16_t height = (Base == Hexadecimal) ? 2
                                      : (Base == Decimal)   ? 2
                                      : (Base == Octal)     ? 3
                                      : (Base == Binary)    ? 8
                                                            : /* default */ 2;
    static constexpr int16_t columnGap = 2;
    static constexpr int16_t lineGap = 1;
    static constexpr int16_t line = lineGap + ValueView::line +
                                    ValueView::height +
                                    ((Base == Hexadecimal) ? 0
                                     : (Base == Decimal)   ? 2
                                     : (Base == Octal)     ? 4
                                     : (Base == Binary)    ? 7
                                                           : 0);
    static constexpr int16_t lineWidth = 30;
    static constexpr int16_t lineHeight = 2;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharWidth = 6;

    /**
     * @brief 6x8 font.
     *
     */
    static constexpr size_t CharHeight = 8;

    /**
     * @brief The number of glyphs to skip at the beginning of the header line.
     *
     */
    static constexpr int headerSkip = 2;

    /**
     * @brief The gap between the header and the number.
     *
     */
    static constexpr int numberGap = 2;

  private:
    static constexpr HeaderGlyphArray6x8
    MakeHeader(void) {
        if constexpr (Base == Hexadecimal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8HH), Glyph(Font6x8EH),
                                       Glyph(Font6x8XH));
        } else if constexpr (Base == Decimal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8DH), Glyph(Font6x8EH),
                                       Glyph(Font6x8CH));
        } else if constexpr (Base == Octal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8OH), Glyph(Font6x8CH),
                                       Glyph(Font6x8TH));
        } else {
            return HeaderGlyphArray6x8(Glyph(Font6x8BH), Glyph(Font6x8IH),
                                       Glyph(Font6x8NH));
        }
    }

    static constexpr HeaderGlyphArray6x8 header = MakeHeader();

    ViewModel &vm;
    bool selected;

    void printHeader(void) const;
    void printNumber(void) const;
};

using HexView = TranscodeView<Hexadecimal>;
using DecView = TranscodeView<Decimal>;
using OctView = TranscodeView<Octal>;
using BinView = TranscodeView<Binary>;

class IndicatorView : public MainView<IndicatorView, AlignLeft> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    IndicatorView(MainDisplay &display, ViewModel &vm)
        : MainView(Area(barOffsetX, indicatorAreaY, TileWidth,
                        indicatorAreaHeight * TileHeight),
                   display),
          vm(vm), currentBase(vm.GetNumberBase()) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

  private:
    static constexpr int16_t barOffsetX = 2;
    static constexpr int16_t indicatorAreaY = 8 * TileHeight;
    static constexpr int16_t indicatorAreaHeight = 15;

    ViewModel &vm;
    NumberBase currentBase;

    int16_t getIndicatorY(NumberBase base) const;
};

template <class Derived>
class SubView : public BasicView<Derived, SubDisplay> {
  public:
    explicit SubView(SubDisplay &display)
        : BasicView<Derived, SubDisplay>(display) {}
};

class InputView : public SubView<InputView> {
  public:
    InputView(SubDisplay &display, ViewModel &vm);

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

  private:
    static constexpr size_t colNum = 5;
    static constexpr size_t rowNum = 7;

    ViewModel &vm;
    TouchScreenHandler<colNum, rowNum> handler;
    TouchButton *numberButtons[16];
    TouchButton *rightBracketButton;

    Sprite<SubDisplay> *leftBracketSprites[2];
    int leftBracketCount;

    void handleBaseChange(void);
    void updateLBrackCount(int count);

    TouchButton &getFocus(Point position, Direction dir);
};

}; // namespace HexCalc
