/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "event.h"
#include "font.h"
#include "number.h"

namespace HexCalc {

/**
 * @brief The alignment of the view text.
 *
 */
enum ViewAlign : uint8_t {
    AlignLeft,
    AlignRight,
};

/**
 * @brief The area of a view. all values are int16_t, is enough for the current
 * screen resolution (256x192).
 *
 */
struct Area {
    /** position x, can be negative */
    int16_t x;
    /** position y, can be negative */
    int16_t y;
    /** area width, must > 0 */
    uint8_t w;
    /** area height, must > 0 */
    uint8_t h;

    static Area
    AreaByPoints(int16_t x1, int16_t x2, int16_t y1, int16_t y2) {
        auto x_min = std::min(x1, x2);
        auto x_max = std::max(x1, x2);
        auto y_min = std::min(y1, y2);
        auto y_max = std::max(y1, y2);

        // The area width and height must be less than 256, otherwise it cannot
        // be represented by uint8_t.
        assert(x_max - x_min <= std::numeric_limits<uint8_t>::max());
        assert(y_max - y_min <= std::numeric_limits<uint8_t>::max());

        auto width = static_cast<uint8_t>(x_max - x_min);
        auto height = static_cast<uint8_t>(y_max - y_min);

        Area area(x_min, y_min, width, height);

        return area;
    }

    Area(int16_t x, int16_t y, uint8_t w, uint8_t h) {
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

class BasicView {
  public:
    BasicView(Area area, ViewAlign align)
        : viewArea(area), viewAlign(align),
          // Initially, the view needs to be rendered at least once.
          dirty(true) {}

    void
    Update(void) {
        dirty = false;
    }

  private:
    Area viewArea;
    ViewAlign viewAlign;
    bool dirty;

    void
    MarkDirty(void) {
        dirty = true;
    }
};

class FormulaView : public BasicView {
  public:
    FormulaView(Area area, ViewAlign align) : BasicView(area, align) {}
};

class ValueView : public BasicView {
  public:
    ValueView(Area area, ViewAlign align) : BasicView(area, align) {}
};

template <NumberBase base>
class TranscodeView : public BasicView {
  public:
    TranscodeView(Area area, ViewAlign align) : BasicView(area, align) {}

  private:
    template <NumberBase>
    struct HeaderTraits {
        static constexpr FontType font0 = FontEmpty;
        static constexpr FontType font1 = FontEmpty;
        static constexpr FontType font2 = FontEmpty;
    };

    static constexpr Glyph header[4] = {
        Glyph::From(HeaderTraits<base>::font0),
        Glyph::From(HeaderTraits<base>::font1),
        Glyph::From(HeaderTraits<base>::font2),
        InvalidGlyph,
    };
};

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

class InputView {
  public:
    InputView() {}
};

}; // namespace HexCalc
