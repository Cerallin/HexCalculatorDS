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
#include "viewmodel.h"

namespace HexCalc {

template <size_t N>
class GlyphArray {
  public:
    constexpr GlyphArray(void) : glyphs{}, size(0) {}

    bool
    Insert(const Glyph &glyph) {
        if (size >= N) {
            return false;
        }

        glyphs[size++] = glyph;

        return true;
    }

    size_t
    Size(void) const {
        return size;
    }

    using iterator = Glyph *;
    using const_iterator = const Glyph *;

    const Glyph &
    operator[](size_t index) const {
        return glyphs[index];
    }

    iterator
    begin() {
        return glyphs;
    }
    iterator
    end() {
        return glyphs + size;
    }

    const_iterator
    begin() const {
        return glyphs;
    }
    const_iterator
    end() const {
        return glyphs + size;
    }

    const_iterator
    cbegin() const {
        return glyphs;
    }
    const_iterator
    cend() const {
        return glyphs + size;
    }

  protected:
    Glyph glyphs[N];
    size_t size;
};

class HeaderGlyphArray6x8 : public GlyphArray<3> {
  public:
    constexpr HeaderGlyphArray6x8(const Glyph &h1, const Glyph &h2,
                                  const Glyph &h3)
        : GlyphArray<3>() {
        this->glyphs[0] = h1;
        this->glyphs[1] = h2;
        this->glyphs[2] = h3;
        this->size = 3;
    }

    static constexpr int CharWidth = 6;
    static constexpr int CharHeight = 8;
};

template <size_t N>
class GlyphArray6x8 : public GlyphArray<N> {
  public:
    constexpr GlyphArray6x8(const GlyphArray<N> &glyphArray)
        : GlyphArray<N>(glyphArray), isNegative(false) {}
    constexpr GlyphArray6x8(DigitArray<N> digits, bool reverse = false)
        : GlyphArray<N>(), isNegative(digits.isNegative) {
        if (reverse) {
            for (size_t i = 0; i < digits.size; i++) {
                this->Insert(
                    Glyph(static_cast<FontType>(digits[i] + Font6x8Zero)));
            }
        } else {
            for (size_t i = 0; i < digits.size; i++) {
                this->Insert(Glyph(static_cast<FontType>(
                    digits[digits.size - 1 - i] + Font6x8Zero)));
            }
        }
    }

    bool
    Negative(void) const {
        return isNegative;
    }

    static constexpr int CharWidth = 6;
    static constexpr int CharHeight = 8;

  private:
    bool isNegative;
};

template <size_t N>
class GlyphArray8x8 : public GlyphArray<N> {
  public:
    constexpr GlyphArray8x8(const GlyphArray<N> &glyphArray)
        : GlyphArray<N>(glyphArray), isNegative(false) {}
    constexpr GlyphArray8x8(DigitArray<N> digits, bool reverse = false)
        : GlyphArray<N>(), isNegative(digits.isNegative) {
        // FIXME negative sign is only for digits, not for glyphs
        if (reverse) {
            for (size_t i = 0; i < digits.size; i++) {
                if (!this->Insert(Glyph(
                        static_cast<FontType>(digits[i] + Font8x8Zero)))) {
                    break;
                }
            }
        } else {
            for (size_t i = 0; i < digits.size; i++) {
                if (!this->Insert(Glyph(static_cast<FontType>(
                        digits[digits.size - 1 - i] + Font8x8Zero)))) {
                    break;
                }
            }
        }
    }

    static constexpr int CharWidth = 8;
    static constexpr int CharHeight = 8;

  protected:
    bool isNegative;
};

/**
 * @brief Calculate the size of a formatted glyph array for a given number of
 * digits and group size.
 *
 * Examples:
 * Hexadecimal: DEADBEAF -> DEAD_BEAF (group size 4, 1 separator)
 * Decimal: -1234567890 -> -1_234_567_890 (group size 3, 2 separators + 1 sign)
 *
 * @param digitCount The number of digits in the number
 * @param groupSize The size of each group of digits (e.g. 4 for binary, 3 for
 * octal, etc.)
 * @return constexpr size_t The size of the formatted glyph array
 */
constexpr size_t
GlyphFormatSize(size_t digitCount, int groupSize, bool hasSign = false) {
    if (groupSize <= 0) {
        return digitCount;
    }
    size_t separatorCount = (digitCount - 1) / groupSize;
    // +1 for potential sign
    return digitCount + separatorCount + (hasSign ? 1 : 0);
}

/**
 * @brief Glyph array for formatted numbers, with separators and optional sign.
 *
 * @tparam Separator The font character used as a separator (e.g. ',' for
 * decimal, '_' for hexadecimal and octal)
 * @tparam GroupSize The size of each group of digits (e.g. 4 for binary, 3 for
 * octal, etc.)
 * @tparam hasSign Whether the number has a sign (true for decimal, false for
 * hexadecimal and octal)
 * @tparam N The maximum number of digits in the number
 */
template <FontType Separator, int GroupSize, bool hasSign, size_t N>
class GlyphFormatArray6x8
    : public GlyphArray<GlyphFormatSize(N, GroupSize, hasSign)> {
  public:
    constexpr GlyphFormatArray6x8(const GlyphArray6x8<N> &glyphArray)
        : GlyphArray<GlyphFormatSize(N, GroupSize, hasSign)>() {
        // Insert digits with separators and sign if needed
        if (!hasSign) {
            auto zerosCount =
                (GroupSize - (glyphArray.Size() % GroupSize)) % GroupSize;
            for (int i = 0; i < zerosCount; i++) {
                this->Insert(Glyph(Font6x8Zero));
            }
        }
        auto negative = glyphArray.Negative();
        size_t starter = (hasSign && negative) ? 1 : 0;
        // Skip sign if present
        for (size_t i = starter; i < glyphArray.Size(); i++) {
            this->Insert(glyphArray[i]);
            bool isLast = (i == glyphArray.Size() - 1);
            if (!isLast && (glyphArray.Size() - i) % GroupSize == 1) {
                this->Insert(Glyph(Separator));
            }
        }
    }

    static constexpr int CharWidth = 6;
    static constexpr int CharHeight = 8;
};

/**
 * @brief Glyph array for hexadecimal numbers, with group size of 4 and
 * separator '_'.
 *
 */
using HexGlyphArray6x8 =
    GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxHexDigits>;
/**
 * @brief Glyph array for decimal numbers, with a sign and group size of 3 and
 * separator ','.
 *
 */
using DecGlyphArray6x8 =
    GlyphFormatArray6x8<Font6x8Comma, 3, true, Number::MaxDecDigits>;

/**
 * @brief Glyph array for octal numbers, with group size of 3 and separator '_'.
 *
 */
using OctGlyphArray6x8 =
    GlyphFormatArray6x8<FontEmpty, 3, false, Number::MaxOctDigits>;

/**
 * @brief Glyph array for binary numbers, with group size of 4 and separator
 * '_'.
 *
 */
class BinGlyphArray6x8
    : public GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxBinDigits> {
  public:
    constexpr BinGlyphArray6x8(
        const GlyphArray6x8<Number::MaxBinDigits> &glyphArray)
        : GlyphFormatArray6x8<FontEmpty, 4, false, Number::MaxBinDigits>(
              GlyphFormatArray6x8<FontEmpty, 16, false, Number::MaxBinDigits>(
                  glyphArray)) {}
};

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

    static constexpr auto viewAlign = Align;

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
    static constexpr size_t maxGlyphs = 6;

    GlyphArray<maxGlyphs> getGlyphs();
};

class FormulaView : public MainView<FormulaView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    FormulaView(MainDisplay &display, const ViewModel &vm)
        : MainView(Area(offsetX, line * TileHeight, lineWidth * TileWidth,
                        height * TileHeight),
                   display),
          vm(vm) {}

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
    const ViewModel &vm;
};

class ValueView : public MainView<ValueView, AlignRight> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    ValueView(MainDisplay &display, const ViewModel &vm)
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
    const ViewModel &vm;
};

template <NumberBase base>
class TranscodeView : public MainView<TranscodeView<base>, AlignLeft> {
  public:
    static constexpr auto TileHeight = MainDisplay::TileHeight;
    static constexpr auto TileWidth = MainDisplay::TileWidth;

    TranscodeView(MainDisplay &display, const ViewModel &vm)
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

    const ViewModel &vm;
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

template <typename Class>
class SubView : public BasicView<Class, SubDisplay> {
  public:
    SubView(SubDisplay &display) : BasicView<Class, SubDisplay>(display) {}
};

class InputView : public SubView<InputView> {
  public:
    InputView(SubDisplay &display);

    EventResult HandleEvent(const Event &e);

    void ForceUpdate(void);
};

}; // namespace HexCalc
