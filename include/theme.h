/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "project.h"

#include "subscreenAreaColors.h"

// for a 4bpp palette, the maximum number of colors is 16 (2^4)
#define MAX_4BPP_PAL_COUNT 16
// for an 8bpp palette, the maximum number of colors is 256 (2^8)
#define MAX_8BPP_PAL_COUNT 256

namespace HexCalc {

constexpr int16_t mainPal[] = {
    0x0000,
    COLOR_COMMON_BG,
    COLOR_COMMON_BORDER,
    COLOR_COMMON_TEXT,
    COLOR_COMMON_SHADOW,
    COLOR_DISABLED_BG,
    COLOR_DISABLED_SHADOW,
    COLOR_DISABLED_BORDER,
};

constexpr int16_t subPal[] = {
    0x0000,
    COLOR_COMMON_BG,
    COLOR_COMMON_BORDER,
    COLOR_COMMON_TEXT,
    COLOR_COMMON_SHADOW,
    COLOR_DISABLED_BG,
    COLOR_DISABLED_SHADOW,
    COLOR_DISABLED_BORDER,
};

constexpr int16_t subSpritePal[] = {
    0x0000,
    COLOR_COMMON_BG,
    COLOR_COMMON_BORDER,
    COLOR_COMMON_TEXT,
    COLOR_COMMON_SHADOW,
    COLOR_DISABLED_BG,
    COLOR_DISABLED_SHADOW,
    COLOR_DISABLED_BORDER,
};

}; // namespace HexCalc
