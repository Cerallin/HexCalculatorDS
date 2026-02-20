/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewhost.h"

using namespace HexCalc;

ViewHost::ViewHost(ViewModel &viewModel)
    : mainDisplay(), subDisplay(), configView(mainDisplay),
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

    bus.Subscribe(configView);
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
    configView.Update();
    formulaView.Update();
    valueView.Update();
    hexView.Update();
    decView.Update();
    octView.Update();
    binView.Update();
    inputView.Update();

    bgUpdate();
}
