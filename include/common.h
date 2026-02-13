/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 * 
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#ifndef COMMON_H
#define COMMON_H

#include <nds.h>

#if (_LIBNDS_MAJOR_ == 2) && (_LIBNDS_MINOR_ == 0)
// version check passed
#else
#warning "This project is writter for libnds v2.0.0+!"
#endif

#endif // COMMON_H
