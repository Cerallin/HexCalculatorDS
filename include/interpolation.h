/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

namespace HexCalc {

/**
 * @brief Maps an animation phase into a blend weight in [0, 256].
 */
using WeightCurve = uint16_t (*)(uint16_t phase, uint16_t period);

/**
 * @brief Color interpolation along a weight curve over a fixed period.
 *
 * At(phase) returns LerpRgb15(from, to, weight(phase, period)).
 */
class ColorCurve {
  public:
    ColorCurve(uint16_t from, uint16_t to, uint16_t period, WeightCurve weight);

    uint16_t Period(void) const;
    uint16_t At(uint16_t phase) const;

  private:
    uint16_t from;
    uint16_t to;
    uint16_t period;
    WeightCurve weight;
};

/**
 * @brief Linear interpolate two RGB15 colors. t256 is in [0, 256].
 */
uint16_t LerpRgb15(uint16_t from, uint16_t to, uint16_t t256);

/**
 * @brief Linear interpolate two unsigned 16-bit values. t256 is in [0, 256].
 */
uint16_t LerpU16(uint16_t from, uint16_t to, uint16_t t256);

/**
 * @brief Triangle wave: 0 -> 256 -> 0 over one period (linear).
 */
uint16_t LinearTriangle(uint16_t phase, uint16_t period);

/**
 * @brief 1D quadratic Bezier from 0 to 256 with one control weight.
 *
 * B(t) = 2(1-t)t*control + t^2*256, t = t256/256.
 */
uint16_t QuadraticBezier(uint16_t t256, uint16_t control);

/**
 * @brief 1D cubic Bezier from 0 to 256 with two control weights.
 *
 * B(t) = 3(1-t)^2*t*c1 + 3(1-t)*t^2*c2 + t^3*256, t = t256/256.
 */
uint16_t CubicBezier(uint16_t t256, uint16_t c1, uint16_t c2);

/**
 * @brief Ease-in-out cubic Bezier over t256 in [0, 256] (fixed controls).
 */
uint16_t EaseInOutCubicBezier(uint16_t t256);

/**
 * @brief Ease-in cubic Bezier over t256 in [0, 256].
 */
uint16_t EaseInCubicBezier(uint16_t t256);

/**
 * @brief Ease-out cubic Bezier over t256 in [0, 256].
 */
uint16_t EaseOutCubicBezier(uint16_t t256);

/**
 * @brief Triangle wave like LinearTriangle, but each half uses ease-in-out
 *        cubic Bezier instead of a straight ramp.
 */
uint16_t BezierTriangle(uint16_t phase, uint16_t period);

}; // namespace HexCalc
