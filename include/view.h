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
#include "font.h"
#include "input.h"
#include "model.h"
#include "number.h"

namespace HexCalc {

template <typename DisplayType>
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
        dirty = false;
    }

  protected:
    DisplayType &display;

    void
    MarkDirty(void) {
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
template <ViewAlign Align>
class MainView : public BasicView<MainDisplay> {
  public:
    MainView(Area area, MainDisplay &display)
        : BasicView(display), viewArea(area) {}

  private:
    Area viewArea;
};

class FormulaView : public MainView<AlignRight> {
  public:
    FormulaView(MainDisplay &display, const FormulaModel &model)
        : MainView(Area(line, 0, 30, height), display), model(model) {}

    EventResult HandleEvent(const Event &e);

    static constexpr int16_t height = 2;
    static constexpr int16_t line = 2;

  private:
    const FormulaModel &model;
};

class ValueView : public MainView<AlignRight> {
  public:
    ValueView(MainDisplay &display, const ValueModel &model)
        : MainView(Area(line, 0, 30, height), display), model(model) {}

    EventResult HandleEvent(const Event &e);

    static constexpr int16_t height = 3;
    static constexpr int16_t line = 2 + FormulaView::height;

  private:
    const ValueModel &model;
};

template <NumberBase base>
class TranscodeView : public MainView<AlignLeft> {
  public:
    TranscodeView(MainDisplay &display, const ValueModel &model)
        : MainView(Area(line, 0, 30, height), display), model(model) {}

    EventResult HandleEvent(const Event &e);

    // hex: 2, dec: 2, oct: 3, bin: 8
    static constexpr int16_t height = (base == Hexadecimal) ? 2
                                      : (base == Decimal)   ? 2
                                      : (base == Octal)     ? 3
                                      : (base == Binary)    ? 8
                                                            : /* default */ 2;
    static constexpr int16_t gap = 1;
    static constexpr int16_t line = gap + ValueView::line + ValueView::height +
                                    ((base == Hexadecimal) ? 0
                                     : (base == Decimal)   ? 2
                                     : (base == Octal)     ? 4
                                     : (base == Binary)    ? 7
                                                           : 0);

  private:
    EventResult HandleValueChanged(void);

    const ValueModel &model;

    template <NumberBase>
    struct HeaderTraits {
        static constexpr FontType font0 = FontEmpty;
        static constexpr FontType font1 = FontEmpty;
        static constexpr FontType font2 = FontEmpty;
    };

    static constexpr Glyph header[4] = {
        Glyph(HeaderTraits<base>::font0),
        Glyph(HeaderTraits<base>::font1),
        Glyph(HeaderTraits<base>::font2),
        InvalidGlyph,
    };
};

using HexView = TranscodeView<Hexadecimal>;
using DecView = TranscodeView<Decimal>;
using OctView = TranscodeView<Octal>;
using BinView = TranscodeView<Binary>;

template <>
template <>
struct TranscodeView<Hexadecimal>::HeaderTraits<Hexadecimal> {
    static constexpr FontType font0 = Font6x8HH;
    static constexpr FontType font1 = Font6x8EH;
    static constexpr FontType font2 = Font6x8XH;
};

template <>
template <>
struct TranscodeView<Decimal>::HeaderTraits<Decimal> {
    static constexpr FontType font0 = Font6x8DH;
    static constexpr FontType font1 = Font6x8EH;
    static constexpr FontType font2 = Font6x8CH;
};

template <>
template <>
struct TranscodeView<Octal>::HeaderTraits<Octal> {
    static constexpr FontType font0 = Font6x8OH;
    static constexpr FontType font1 = Font6x8CH;
    static constexpr FontType font2 = Font6x8TH;
};

template <>
template <>
struct TranscodeView<Binary>::HeaderTraits<Binary> {
    static constexpr FontType font0 = Font6x8BH;
    static constexpr FontType font1 = Font6x8IH;
    static constexpr FontType font2 = Font6x8NH;
};

class SubView : public BasicView<SubDisplay> {
  public:
    SubView(SubDisplay &display) : BasicView(display) {}
};

class InputView : public SubView {
  public:
    InputView(SubDisplay &display) : SubView(display) {}

    EventResult HandleEvent(const Event &e);
};

}; // namespace HexCalc
