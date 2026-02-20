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

template <typename Class, typename DisplayType>
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
            static_cast<Class *>(this)->ForceUpdate();
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
template <typename Class, ViewAlign Align>
class MainView : public BasicView<Class, MainDisplay> {
  public:
    MainView(Area area, MainDisplay &display)
        : BasicView<Class, MainDisplay>(display), viewArea(area) {}

  protected:
    Area viewArea;
};

class ConfigView : public MainView<ConfigView, AlignLeft> {
  public:
    ConfigView(MainDisplay &display)
        : MainView(Area(offsetX, line * MainDisplay::TileHeight,
                        lineWidth * MainDisplay::TileWidth,
                        height * MainDisplay::TileHeight),
                   display) {}

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
};

class FormulaView : public MainView<FormulaView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    FormulaView(MainDisplay &display, const FormulaModel &model)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          model(model) {}

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
    const FormulaModel &model;
};

class ValueView : public MainView<ValueView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    ValueView(MainDisplay &display, const ValueModel &model)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          model(model) {}

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
    const ValueModel &model;
};

template <NumberBase base>
class TranscodeView : public MainView<TranscodeView<base>, AlignLeft> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    TranscodeView(MainDisplay &display, const ValueModel &model)
        : MainView<TranscodeView<base>, AlignLeft>(
              //
              Area(0, line * TileHeight, lineWidth * TileWidth,
                   height * TileHeight),
              display),
          model(model), selected(false) {}

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
    template <NumberBase>
    struct HeaderTraits {
        static constexpr FontType font0 = FontEmpty;
        static constexpr FontType font1 = FontEmpty;
        static constexpr FontType font2 = FontEmpty;
    };

    static constexpr Glyph header[] = {
        Glyph(HeaderTraits<base>::font0),
        Glyph(HeaderTraits<base>::font1),
        Glyph(HeaderTraits<base>::font2),
    };
    static constexpr int headerLength = sizeof(header) / sizeof(Glyph);

    static constexpr int barOffsetX = 2;

    const ValueModel &model;
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

template <typename Class>
class SubView : public BasicView<Class, SubDisplay> {
  public:
    SubView(SubDisplay &display) : BasicView<Class, SubDisplay>(display) {}
};

class InputView : public SubView<InputView> {
  public:
    InputView(SubDisplay &display) : SubView(display) {}

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);
};

}; // namespace HexCalc
