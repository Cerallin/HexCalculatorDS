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
