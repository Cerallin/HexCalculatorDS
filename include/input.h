/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

#include <algorithm>
#include <limits>

namespace HexCalc {

struct Point {
    int16_t x;
    int16_t y;

    constexpr Point(int16_t px, int16_t py) : x(px), y(py) {}

    constexpr Point(touchPosition touchPosition)
        : x(static_cast<int16_t>(touchPosition.px)),
          y(static_cast<int16_t>(touchPosition.py)) {}

    constexpr Point(int data)
        : x(static_cast<int16_t>(data >> 16)),
          y(static_cast<int16_t>(data & 0xFFFF)) {}

    int
    ToInt() const {
        return (static_cast<int>(x) << 16) | static_cast<int>(y);
    }
};

struct Area {
    int16_t x;
    int16_t y;
    uint8_t w;
    uint8_t h;

    Area(Point p1, Point p2) {
        auto x_min = std::min(p1.x, p2.x);
        auto x_max = std::max(p1.x, p2.x);
        auto y_min = std::min(p1.y, p2.y);
        auto y_max = std::max(p1.y, p2.y);

        assert(x_max - x_min <= std::numeric_limits<uint8_t>::max());
        assert(y_max - y_min <= std::numeric_limits<uint8_t>::max());

        auto width = static_cast<uint8_t>(x_max - x_min);
        auto height = static_cast<uint8_t>(y_max - y_min);

        x = x_min;
        y = y_min;
        w = width;
        h = height;
    }

    Area(int16_t px, int16_t py, uint8_t width, uint8_t height)
        : x(px), y(py), w(width), h(height) {}
};

struct KeyInput {
    uint32_t keys;

    bool
    Active(void) const {
        return keys != 0;
    }
};

struct TouchInput {
    Point point;
    bool pressed;

    bool
    Active(void) const {
        return pressed;
    }
};

inline KeyInput
ReadKeyInput(void) {
    scanKeys();
    KeyInput input{keysDownRepeat()};
    return input;
}

inline TouchInput
ReadTouchInput(void) {
    touchPosition touchPos{};
    touchRead(&touchPos);
    bool pressed = (touchPos.px != 0) || (touchPos.py != 0);
    TouchInput input{Point(touchPos), pressed};
    return input;
}

}; // namespace HexCalc
