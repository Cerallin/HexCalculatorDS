/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "input.h"
#include "viewhost.h"
#include "viewmodel.h"

#if (_LIBNDS_MAJOR_ == 2)
#error "This project is not compatable to libnds v2.0.0+!"
#endif

static inline void
debugInit(void) {
#ifndef NDEBUG
    consoleDebugInit(DebugDevice_NOCASH);
#endif
}

int
main(void) {
    // debug console
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
