/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewhost.h"

using namespace HexCalc;

InputViewAdapter::InputViewAdapter(SubDisplay &subDisplay, ViewModel &viewModel)
    : SubView(subDisplay, viewModel), inputView(subDisplay, viewModel),
      editorView(subDisplay, viewModel), drawerView(subDisplay, viewModel),
      state(InputState), shouldSwitchView(true) {}

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

        Invalidate();
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
