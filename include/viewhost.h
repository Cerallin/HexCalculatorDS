/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "display.h"
#include "view.h"
#include "viewmodel.h"

#include <type_traits>

namespace HexCalc {

/**
 * @brief The InputViewAdapter class manages the views on the sub screen.
 *
 */
class InputViewAdapter : private SubView<InputViewAdapter> {
  public:
    explicit InputViewAdapter(SubDisplay &subDisplay, ViewModel &viewModel);

    /**
     * @brief Update the currently active view.
     *
     */
    void Update(void);

    EventResult HandleEvent(const Event &e);

  private:
    InputView inputView;
    EditorView editorView;
    DrawerView drawerView;

    enum {
        InputState,
        EditorState,
    } state;

    bool shouldSwitchView;
};

/**
 * @brief Compile-time set of view types claimed by AnimationHost
 *        (skipped in ViewHost Subscribe/Update).
 *
 * Usage: ViewClaims<FormulaView, ValueView, ...>
 */
template <typename... Claimed>
struct ViewClaims {
    template <typename V>
    struct IsClaimed
        : std::bool_constant<(std::is_same_v<V, Claimed> || ... || false)> {};
};

/** @brief Default claims: no view is taken over by AnimationHost. */
using NoViewClaims = ViewClaims<>;

/**
 * @brief The ViewHost class manages the views on the main and sub screens. It
 * is responsible for updating the views when the models change and handling
 * user inputs by dispatching events to the appropriate views. It also
 * initializes the displays and registers the views with the ViewModel.
 *
 * @tparam Claims Compile-time policy for views owned by AnimationHost.
 *         Claimed view types are skipped in Subscribe/Update.
 */
template <typename Claims = NoViewClaims>
class ViewHost : private NonCopyable {
  public:
    explicit ViewHost(ViewModel &viewModel)
        : mainDisplay(), subDisplay(), configView(mainDisplay, viewModel),
          formulaView(mainDisplay, viewModel),
          valueView(mainDisplay, viewModel), hexView(mainDisplay, viewModel),
          decView(mainDisplay, viewModel), octView(mainDisplay, viewModel),
          binView(mainDisplay, viewModel),
          indicatorView(mainDisplay, viewModel),
          inputViewAdapter(subDisplay, viewModel) {
        // Register views
        auto &bus = viewModel.Bus();

        subscribeUnlessClaimed(bus, configView);
        subscribeUnlessClaimed(bus, formulaView);
        subscribeUnlessClaimed(bus, valueView);
        subscribeUnlessClaimed(bus, hexView);
        subscribeUnlessClaimed(bus, decView);
        subscribeUnlessClaimed(bus, octView);
        subscribeUnlessClaimed(bus, binView);
        subscribeUnlessClaimed(bus, indicatorView);
        subscribeUnlessClaimed(bus, inputViewAdapter);
    }

    void
    Update(void) {
        updateUnlessClaimed(configView);
        updateUnlessClaimed(formulaView);
        updateUnlessClaimed(valueView);
        updateUnlessClaimed(hexView);
        updateUnlessClaimed(decView);
        updateUnlessClaimed(octView);
        updateUnlessClaimed(binView);
        updateUnlessClaimed(indicatorView);
        updateUnlessClaimed(inputViewAdapter);

        // Must be called once per frame --said libnds
        bgUpdate();

        subDisplay.UpdateSprites();
    }

    MainDisplay &
    GetMainDisplay(void) {
        return mainDisplay;
    }

    SubDisplay &
    GetSubDisplay(void) {
        return subDisplay;
    }

    ConfigView &
    GetConfigView(void) {
        return configView;
    }

    FormulaView &
    GetFormulaView(void) {
        return formulaView;
    }

    ValueView &
    GetValueView(void) {
        return valueView;
    }

    TranscodeView<Hexadecimal> &
    GetHexView(void) {
        return hexView;
    }

    TranscodeView<Decimal> &
    GetDecView(void) {
        return decView;
    }

    TranscodeView<Octal> &
    GetOctView(void) {
        return octView;
    }

    TranscodeView<Binary> &
    GetBinView(void) {
        return binView;
    }

    IndicatorView &
    GetIndicatorView(void) {
        return indicatorView;
    }

    InputViewAdapter &
    GetInputViewAdapter(void) {
        return inputViewAdapter;
    }

  private:
    MainDisplay mainDisplay;
    SubDisplay subDisplay;

    ConfigView configView;
    FormulaView formulaView;
    ValueView valueView;
    TranscodeView<Hexadecimal> hexView;
    TranscodeView<Decimal> decView;
    TranscodeView<Octal> octView;
    TranscodeView<Binary> binView;
    IndicatorView indicatorView;
    InputViewAdapter inputViewAdapter;

    template <typename V>
    void
    subscribeUnlessClaimed(EventBus &bus, V &view) {
        if constexpr (!Claims::template IsClaimed<V>::value) {
            bus.Subscribe(view);
        }
    }

    template <typename V>
    void
    updateUnlessClaimed(V &view) {
        if constexpr (!Claims::template IsClaimed<V>::value) {
            view.Update();
        }
    }
};

}; // namespace HexCalc
