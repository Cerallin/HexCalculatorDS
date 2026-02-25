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
#include "model.h"
#include "viewmodel.h"

namespace HexCalc {

template <class Derived, typename DisplayType>
class BasicView {
  public:
    // Initially, the view needs to be rendered at least once.
    BasicView(DisplayType &display) : dirty(true), display(display) {}

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

    template <class FormatArrayType, class GlyphArrayType, size_t N>
    void
    PrintGlyphs(DigitArray<N> digits, bool underline = false) const {
        Point start(viewArea.x, viewArea.y);
        if constexpr (Derived::viewAlign == AlignLeft) {
            GlyphArrayType glyphs(digits, true);
            FormatArrayType formattedGlyphs(glyphs);
            this->display.ClearLine(start, formattedGlyphs.CharWidth,
                                    underline);
            this->display.PrintLine(formattedGlyphs, start);
        } else { // align right
            GlyphArrayType glyphs(digits, false);
            FormatArrayType formattedGlyphs(glyphs);
            Area8x8 area(viewArea);
            auto skipGlyphs = area.w - formattedGlyphs.Size();
            Point glyphStart(viewArea.x +
                                 (skipGlyphs * formattedGlyphs.CharWidth),
                             viewArea.y);
            this->display.ClearLine(start, glyphs.CharWidth, underline);
            this->display.PrintLine(formattedGlyphs, glyphStart);
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
     * @brief Clear the glyphs in the formula view.
     *
     */
    void clear(void);

    /**
     * @brief Bake the formula glyphs from the formula model to the view's glyph
     * array.
     *
     */
    void bake(void);
};

class ValueView : public MainView<ValueView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    ValueView(MainDisplay &display, ViewModel &vm)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          vm(vm) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

    static constexpr int16_t offsetX = 2;

    static constexpr int16_t height = 3;
    static constexpr int16_t line = 2 + FormulaView::height;
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
    ViewModel &vm;
};

template <NumberBase base>
class TranscodeView : public MainView<TranscodeView<base>, AlignLeft> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    TranscodeView(MainDisplay &display, ViewModel &vm)
        : MainView<TranscodeView<base>, AlignLeft>(
              //
              Area(0, line * TileHeight, lineWidth * TileWidth,
                   height * TileHeight),
              display),
          vm(vm), selected(false) {}

    EventResult
    HandleEvent(const Event &e) {
        if ((e.type == EventType::UpdateBaseEvent) ||
            (e.type == EventType::ClearEvent)) {
            return handleBaseChanged();
        } else if (e.type == EventType::ValueChangedEvent) {
            return handleValueChanged();
        }

        return Skipped;
    }

    void ForceUpdate(void);

    // hex: 2, dec: 2, oct: 3, bin: 8
    static constexpr int16_t height = (base == Hexadecimal) ? 2
                                      : (base == Decimal)   ? 2
                                      : (base == Octal)     ? 3
                                      : (base == Binary)    ? 8
                                                            : /* default */ 2;
    static constexpr int16_t columnGap = 2;
    static constexpr int16_t lineGap = 1;
    static constexpr int16_t line = lineGap + ValueView::line +
                                    ValueView::height +
                                    ((base == Hexadecimal) ? 0
                                     : (base == Decimal)   ? 2
                                     : (base == Octal)     ? 4
                                     : (base == Binary)    ? 7
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
        if constexpr (base == Hexadecimal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8HH), Glyph(Font6x8EH),
                                       Glyph(Font6x8XH));
        } else if constexpr (base == Decimal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8DH), Glyph(Font6x8EH),
                                       Glyph(Font6x8CH));
        } else if constexpr (base == Octal) {
            return HeaderGlyphArray6x8(Glyph(Font6x8OH), Glyph(Font6x8CH),
                                       Glyph(Font6x8TH));
        } else {
            return HeaderGlyphArray6x8(Glyph(Font6x8BH), Glyph(Font6x8IH),
                                       Glyph(Font6x8NH));
        }
    }

    static constexpr HeaderGlyphArray6x8 header = MakeHeader();

    static constexpr int barOffsetX = 2;

    ViewModel &vm;
    bool selected;

    /**
     * @brief Handle UpdateBase Event.
     *
     * @return EventResult Consumed if the event is handled and the view needs
     * to be updated, Skipped if the event is not relevant to this view.
     */
    EventResult handleBaseChanged(void);

    /**
     * @brief Handle ValueChanged Event.
     *
     * @return EventResult Consumed if the event is handled and the view needs
     * to be updated, Skipped if the event is not relevant to this view.
     */
    EventResult handleValueChanged(void);

    void printHeader(void) const;
    void printNumber(void) const;
};

using HexView = TranscodeView<Hexadecimal>;
using DecView = TranscodeView<Decimal>;
using OctView = TranscodeView<Octal>;
using BinView = TranscodeView<Binary>;

template <class Derived>
class SubView : public BasicView<Derived, SubDisplay> {
  public:
    SubView(SubDisplay &display) : BasicView<Derived, SubDisplay>(display) {}
};

class InputView : public SubView<InputView> {
  public:
    InputView(SubDisplay &display, ViewModel &vm);

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);

  private:
    ViewModel &vm;
    // FIXME magic numbers
    TouchScreenHandler<34> handler;
    TouchButton *numberButtons[16];
    TouchButton *rightBracketButton;

    void handleBaseChange(void);
    void handleWidthChange(void);
    void handleSignChange(void);
};

}; // namespace HexCalc
