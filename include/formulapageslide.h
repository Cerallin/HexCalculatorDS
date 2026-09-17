/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "animation.h"
#include "view.h"
#include "viewmodel.h"

namespace HexCalc::AnimationEffects {

/**
 * @brief Formula page slide: continuous glyph-strip scroll by startIndex delta.
 */
class FormulaPageSlide {
  public:
    explicit FormulaPageSlide(FormulaView &view);

    bool TryHandle(const Event &e, AnimationGate &gate);
    bool Suppress(const Event &e) const;
    bool IsActive(void) const;
    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);
    bool SuppressesViewUpdate(void) const;

  private:
    using StripGlyphs = GlyphArray6x8<FormulaManager::MaxTransitionGlyphs>;

    FormulaView &view;
    const MainDisplay &display;
    const ViewModel &vm;
    Area area;

    bool active;
    Direction dir;
    int16_t offsetPx;
    int16_t distancePx;
    int16_t baseX;
    size_t lastStart;
    StripGlyphs strip;

    bool Tick(void);
    void DrawFrame(void) const;
    void DrawStrip(int16_t xOffset) const;

    static bool TickThunk(void *ctx);
};

}; // namespace HexCalc::AnimationEffects
