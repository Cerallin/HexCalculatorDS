/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewhost.h"

using namespace HexCalc;

ViewHost::ViewHost(ViewModel &viewModel)
    : mainDisplay(), subDisplay(),
      formulaView(mainDisplay, viewModel.Formula()),
      valueView(mainDisplay, viewModel.Value()),
      hexView(mainDisplay, viewModel.Value()),
      decView(mainDisplay, viewModel.Value()),
      octView(mainDisplay, viewModel.Value()),
      binView(mainDisplay, viewModel.Value()), inputView(subDisplay) {
    RegisterViews(viewModel);
}

void
ViewHost::RegisterViews(ViewModel &viewModel) {
    auto &bus = viewModel.Bus();

    bus.Subscribe(formulaView);
    bus.Subscribe(valueView);
    bus.Subscribe(hexView);
    bus.Subscribe(decView);
    bus.Subscribe(octView);
    bus.Subscribe(binView);
    bus.Subscribe(inputView);
}

void
ViewHost::Update(void) {
    // TODO render dirty views instead of drawing placeholder glyphs
    for (uint16_t x = 0; x < 32; x++) {
        for (uint16_t y = 0; y < 12; y++) {
            mainDisplay.PutGlyph(x * 8, y * 16, Glyph(x + y * 32));
        }
    }
    bgUpdate();
}
