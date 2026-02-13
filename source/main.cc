/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "common.h"

int
main(void) {
    // main loop
    while (true) {
        swiWaitForVBlank();
        scanKeys();
    }

    return 0;
}
