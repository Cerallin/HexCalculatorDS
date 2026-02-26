/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "input.h"
#include "viewhost.h"
#include "viewmodel.h"

int
main(void) {
    // debug
    debugInit();

    HexCalc::EventBus eventBus;
    HexCalc::Commands commands(eventBus);
    HexCalc::InputHandler inputHandler(eventBus, commands);
    HexCalc::ViewModel viewModel(eventBus, commands);
    HexCalc::ViewHost viewHost(viewModel);

    // main loop
    while (true) {
        swiWaitForVBlank();

        inputHandler.Update();
        viewModel.DispatchEvents();
        viewHost.Update();
    }

    return 0;
}
