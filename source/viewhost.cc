/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewhost.h"

using namespace HexCalc;

ViewHost::ViewHost(ViewModel &viewModel)
    : mainDisplay(), subDisplay(), configView(mainDisplay, viewModel),
      formulaView(mainDisplay, viewModel), valueView(mainDisplay, viewModel),
      hexView(mainDisplay, viewModel), decView(mainDisplay, viewModel),
      octView(mainDisplay, viewModel), binView(mainDisplay, viewModel),
      indicatorView(mainDisplay, viewModel),
      inputViewAdapter(subDisplay, viewModel) {
    registerViews(viewModel);
}

void
ViewHost::registerViews(ViewModel &viewModel) {
    auto &bus = viewModel.Bus();

    bus.Subscribe(configView);
    bus.Subscribe(formulaView);
    bus.Subscribe(valueView);
    bus.Subscribe(hexView);
    bus.Subscribe(decView);
    bus.Subscribe(octView);
    bus.Subscribe(binView);
    bus.Subscribe(indicatorView);
    bus.Subscribe(inputViewAdapter);
}

void
ViewHost::Update(void) {
    // render dirty views instead of drawing placeholder glyphs
    configView.Update();
    formulaView.Update();
    valueView.Update();
    hexView.Update();
    decView.Update();
    octView.Update();
    binView.Update();
    indicatorView.Update();
    inputViewAdapter.Update();

    // Must be called once per frame --said libnds
    bgUpdate();

    subDisplay.UpdateSprites();
}

InputViewAdapter::InputViewAdapter(SubDisplay &subDisplay, ViewModel &viewModel)
    : SubView(subDisplay), display(subDisplay), vm(viewModel),
      inputView(subDisplay, viewModel), editorView(subDisplay, viewModel),
      drawerView(subDisplay, viewModel), state(InputState),
      shouldSwitchView(true) {}

void
InputViewAdapter::Update(void) {
    if (shouldSwitchView) {
        display.CleanLayers();
        drawerView.Setup();

        if (state == InputState) {
            inputView.Setup();
        } else if (state == EditorState) {
            editorView.Setup();
        } else {
            // should never reach here
        }
        shouldSwitchView = false;
    }

    drawerView.Update();

    if (state == InputState) {
        inputView.Update();
    } else if (state == EditorState) {
        editorView.Update();
    } else {
        // should never reach here
    }
}

EventResult
InputViewAdapter::HandleEvent(const Event &e) {
    drawerView.HandleEvent(e);

    if (e.type == EventType::InputViewChangedEvent) {
        if (state == InputState) {
            inputView.Teardown();
            state = EditorState;
        } else if (state == EditorState) {
            editorView.Teardown();
            state = InputState;
        } else {
            // should never reach here
        }

        markDirty();
        shouldSwitchView = true;

        return Consumed;
    } else {
        if (state == InputState) {
            return inputView.HandleEvent(e);
        } else if (state == EditorState) {
            return editorView.HandleEvent(e);
        } else {
            // should never reach here
            return Failed;
        }
    }
}
