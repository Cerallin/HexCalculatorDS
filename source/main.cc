/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewhost.h"
#include "viewmodel.h"

int
main(void) {
    // debug
    debugInit();
    debugf("Hello world\n");

    HexCalc::ViewModel viewModel;
    HexCalc::ViewHost viewHost(viewModel);

    // main loop
    while (true) {
        swiWaitForVBlank();

        viewModel.HandleInputs();
        viewModel.DispatchEvents();

        viewHost.Update();
    }

    return 0;
}
