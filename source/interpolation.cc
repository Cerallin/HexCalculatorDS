/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "interpolation.h"

namespace HexCalc {

namespace {

constexpr uint16_t kScale = 256;

uint16_t
clampWeight(int64_t value) {
    if (value <= 0) {
        return 0;
    }
    if (value >= kScale) {
        return kScale;
    }
    return static_cast<uint16_t>(value);
}

} // namespace

uint16_t
LerpU16(uint16_t from, uint16_t to, uint16_t t256) {
    if (t256 >= kScale) {
        return to;
    }

    const int delta = static_cast<int>(to) - static_cast<int>(from);
    return static_cast<uint16_t>(from +
                                 (delta * static_cast<int>(t256)) / kScale);
}

int
LerpInt(int from, int to, uint16_t t256) {
    if (t256 >= kScale) {
        return to;
    }

    const int delta = to - from;
    return from + (delta * static_cast<int>(t256)) / kScale;
}

uint16_t
LerpRgb15(uint16_t from, uint16_t to, uint16_t t256) {
    if (t256 >= kScale) {
        return to;
    }

    const int fromR = static_cast<int>(from & 0x1F);
    const int fromG = static_cast<int>((from >> 5) & 0x1F);
    const int fromB = static_cast<int>((from >> 10) & 0x1F);
    const int toR = static_cast<int>(to & 0x1F);
    const int toG = static_cast<int>((to >> 5) & 0x1F);
    const int toB = static_cast<int>((to >> 10) & 0x1F);

    const uint16_t r = static_cast<uint16_t>(
        fromR + ((toR - fromR) * static_cast<int>(t256)) / kScale);
    const uint16_t g = static_cast<uint16_t>(
        fromG + ((toG - fromG) * static_cast<int>(t256)) / kScale);
    const uint16_t b = static_cast<uint16_t>(
        fromB + ((toB - fromB) * static_cast<int>(t256)) / kScale);

    return static_cast<uint16_t>(r | (g << 5) | (b << 10));
}

uint16_t
LinearTriangle(uint16_t phase, uint16_t period) {
    if (period == 0) {
        return 0;
    }

    phase = static_cast<uint16_t>(phase % period);
    const uint16_t half = static_cast<uint16_t>(period / 2);
    if (half == 0) {
        return 0;
    }

    if (phase <= half) {
        return static_cast<uint16_t>((phase * kScale) / half);
    }

    const uint16_t descending = static_cast<uint16_t>(period - phase);
    return static_cast<uint16_t>((descending * kScale) / half);
}

uint16_t
QuadraticBezier(uint16_t t256, uint16_t control) {
    if (t256 == 0) {
        return 0;
    }
    if (t256 >= kScale) {
        return kScale;
    }

    const int64_t u = t256;
    const int64_t inv = kScale - u;
    // B = (2*inv*u*control + u*u*256) / 256^2
    const int64_t num = 2 * inv * u * control + u * u * kScale;
    return clampWeight(num / (static_cast<int64_t>(kScale) * kScale));
}

uint16_t
CubicBezier(uint16_t t256, uint16_t c1, uint16_t c2) {
    if (t256 == 0) {
        return 0;
    }
    if (t256 >= kScale) {
        return kScale;
    }

    const int64_t u = t256;
    const int64_t inv = kScale - u;
    // B = (3*inv^2*u*c1 + 3*inv*u^2*c2 + u^3*256) / 256^3
    const int64_t num =
        3 * inv * inv * u * c1 + 3 * inv * u * u * c2 + u * u * u * kScale;
    const int64_t den = static_cast<int64_t>(kScale) * kScale * kScale;
    return clampWeight(num / den);
}

uint16_t
EaseInOutCubicBezier(uint16_t t256) {
    // Smooth S-curve: slow start and end.
    return CubicBezier(t256, 0, kScale);
}

uint16_t
EaseInCubicBezier(uint16_t t256) {
    // Slow start, fast finish.
    return CubicBezier(t256, 0, 64);
}

uint16_t
EaseOutCubicBezier(uint16_t t256) {
    // Fast start, slow finish.
    return CubicBezier(t256, 192, kScale);
}

uint16_t
BezierTriangle(uint16_t phase, uint16_t period) {
    if (period == 0) {
        return 0;
    }

    phase = static_cast<uint16_t>(phase % period);
    const uint16_t half = static_cast<uint16_t>(period / 2);
    if (half == 0) {
        return 0;
    }

    uint16_t linearT;
    if (phase <= half) {
        linearT = static_cast<uint16_t>((phase * kScale) / half);
    } else {
        const uint16_t descending = static_cast<uint16_t>(period - phase);
        linearT = static_cast<uint16_t>((descending * kScale) / half);
    }

    return EaseInOutCubicBezier(linearT);
}

ColorCurve::ColorCurve(uint16_t from, uint16_t to, uint16_t period,
                       WeightCurve weight)
    : from(from), to(to), period(period), weight(weight) {}

uint16_t
ColorCurve::Period(void) const {
    return period;
}

uint16_t
ColorCurve::At(uint16_t phase) const {
    return LerpRgb15(from, to, weight(phase, period));
}

}; // namespace HexCalc
