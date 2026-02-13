/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include <cstddef>
#include <cstdlib>
#include <cstring>

#include <nds.h>

#if (_LIBNDS_MAJOR_ == 2)
#error "This project is not compatable to libnds v2.0.0+!"
#endif
