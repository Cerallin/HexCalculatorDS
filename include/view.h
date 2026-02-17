/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "event.h"
#include "number.h"

namespace HexCalc {

enum ViewAlign : uint8_t {
    AlignLeft,
    AlignRight,
};

struct Area {
    /** position x */
    int x;
    /** position y */
    int y;

    /** area width */
    int w;
    /** area height */
    int h;

    static Area
    AreaByPoints(int x1, int x2, int y1, int y2) {
        int x_min = std::min(x1, x2);
        int x_max = std::max(x1, x2);
        int y_min = std::min(y1, y2);
        int y_max = std::max(y1, y2);

        Area area(x_min, y_min, (x_max - x_min), (y_max - y_min));

        return area;
    }

    Area(int x, int y, int w, int h) {
        assert((w > 0) && (h > 0));

        x = x;
        y = y;
        w = w;
        h = h;
    }
};

class BasicView {
  public:
    BasicView(Area area, ViewAlign align)
        : viewArea(area), viewAlign(align), dirty(false) {}

    void
    Update(void) {
        dirty = true;
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

    constexpr const char *
    NumberBaseStr() {
        switch (base) {
        case Hexadecimal:
            return "HEX";
        case Decimal:
            return "DEC";
        case Octal:
            return "OCT";
        case Binary:
            return "BIN";
        default:
            return "";
        }
    }
};

class InputView {
  public:
    InputView() {}
};

}; // namespace HexCalc
