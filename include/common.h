/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <array>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <type_traits>
#include <utility>

#define HEXCALC_GCC_UNUSED [[maybe_unused]]

#if defined(__GNUC__) && (defined(__arm__) || defined(__thumb__))
#define HEXCALC_ARM_CODE __attribute__((target("arm")))
#else
#define HEXCALC_ARM_CODE
#endif

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
