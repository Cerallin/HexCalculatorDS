/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewmodel.h"

int
main(void) {
    // debug
    debugInit();
    debugf("Hello world\n");

    auto viewModel = HexCalc::ViewModel();

    // main loop
    while (true) {
        swiWaitForVBlank();

        viewModel.HandleInputs();
        viewModel.DispatchEvents();
        viewModel.UpdateViews();
    }

    return 0;
}
