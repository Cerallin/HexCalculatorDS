/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>

#define HEXCALC_GCC_UNUSED [[maybe_unused]]

static inline int
debugf(const char *fmt, ...) {
    int result = 0;
#ifndef NDEBUG
    va_list args;
    va_start(args, fmt);
    result = vfprintf(stderr, fmt, args);
    va_end(args);
#endif
    return result;
}
