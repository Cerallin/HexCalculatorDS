/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "animation.h"
#include "interpolation.h"
#include "view.h"
#include "viewmodel.h"

namespace HexCalc::AnimationEffects {

/**
 * @brief Default implementations for AnimationEffect::From() methods.
 *        Concrete effects override only what they need.
 */
class BasicEffect {
  public:
    bool
    TryHandle(const Event &, AnimationGate &) {
        return false;
    }

    bool
    Suppress(const Event &) const {
        return false;
    }

    bool
    IsActive(void) const {
        return active;
    }

    void
    AfterHandle(const Event &, AnimationGate &) {}

    void
    Cancel(void) {}

    bool
    SuppressesViewUpdate(void) const {
        return false;
    }

  protected:
    bool active = false;
};

/**
 * @brief Focus sign breathe: lerp sign palette between bg and border while a
 *        keyboard button is focused. Runs on the NonBlocking channel.
 */
class FocusSignBreathe : public BasicEffect {
  public:
    FocusSignBreathe(InputView &inputView, SubDisplay &display);

    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);

  private:
    static constexpr uint16_t Period = 60;

    InputView &inputView;
    SubDisplay &display;
    ColorCurve curve;

    int buttonIndex;
    uint16_t phase;

    void sync(AnimationGate &gate);
    bool Tick(void);

    static bool TickThunk(void *ctx);
    static void CancelThunk(void *ctx);
};

/**
 * @brief Formula page slide: continuous glyph-strip scroll by startIndex delta.
 */
class FormulaPageSlide : public BasicEffect {
  public:
    explicit FormulaPageSlide(FormulaView &view);

    bool TryHandle(const Event &e, AnimationGate &gate);
    bool Suppress(const Event &e) const;
    bool SuppressesViewUpdate(void) const;

  private:
    using StripGlyphs = GlyphArray6x8<FormulaManager::MaxTransitionGlyphs>;

    FormulaView &view;
    const MainDisplay &display;
    const ViewModel &vm;
    Area area;

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

/**
 * @brief Editor shift-mode label slide within the 9x2 tile window.
 */
class ShiftModeSlide : public BasicEffect {
  public:
    ShiftModeSlide(EditorView &editorView);

    bool TryHandle(const Event &e, AnimationGate &gate);
    bool SuppressesViewUpdate(void) const;

  private:
    EditorView &editorView;
    const ViewModel &vm;
    ShiftModeManager &shiftModes;

    Direction dir;
    NumberShiftMode fromMode;
    NumberShiftMode toMode;
    NumberShiftMode lastMode;
    int16_t offsetPx;
    int16_t distancePx;

    bool Tick(void);
    void DrawFrame(void) const;

    static bool TickThunk(void *ctx);
};

/**
 * @brief Shared visual-focus cell for DigitPad focus frame effects.
 */
struct DigitFocusAnimState {
    Point lastCell;

    DigitFocusAnimState(void) : lastCell(-1, -1) {}

    bool
    HasVisual(void) const {
        return (lastCell.x >= 0) && (lastCell.y >= 0);
    }

    void
    Clear(void) {
        lastCell = Point(-1, -1);
    }
};

/**
 * @brief DigitPad focus first-appear: four-corner converge / reticle lock.
 */
class DigitFocusConverge : public BasicEffect {
  public:
    DigitFocusConverge(EditorView &editorView, DigitFocusAnimState &state);

    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);

  private:
    static constexpr uint16_t Frames = 5;
    static constexpr int SpreadPx = 2;

    DigitPad &digitPad;
    DigitFocus &digitFocus;
    DigitFocusAnimState &state;

    uint16_t phase;
    Point toPx;
    Point startCorners[DigitFocus::CornerCount];
    Point endCorners[DigitFocus::CornerCount];

    bool start(AnimationGate &gate);
    bool Tick(void);
    void snapToLogical(void);

    static bool TickThunk(void *ctx);
    static void CancelThunk(void *ctx);
};

/**
 * @brief DigitPad focus cell-to-cell slide (ease-in-out + 1px overshoot).
 */
class DigitFocusSlide : public BasicEffect {
  public:
    DigitFocusSlide(EditorView &editorView, DigitFocusAnimState &state);

    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);

  private:
    static constexpr uint16_t MoveFrames = 5;
    static constexpr uint16_t SettleFrames = 1;
    static constexpr uint16_t TotalFrames = MoveFrames + SettleFrames;

    DigitPad &digitPad;
    DigitFocus &digitFocus;
    DigitFocusAnimState &state;

    uint16_t phase;
    Point fromPx;
    Point toPx;

    bool start(AnimationGate &gate);
    bool Tick(void);
    void snapToLogical(void);

    static bool TickThunk(void *ctx);
    static void CancelThunk(void *ctx);
};

/**
 * @brief DigitPad focus lose: four-corner diverge (inverse of converge) then
 * Hide.
 */
class DigitFocusDiverge : public BasicEffect {
  public:
    DigitFocusDiverge(EditorView &editorView, DigitFocusAnimState &state);

    void AfterHandle(const Event &e, AnimationGate &gate);
    void Cancel(void);

  private:
    static constexpr uint16_t Frames = 5;
    static constexpr int SpreadPx = 2;

    DigitPad &digitPad;
    DigitFocus &digitFocus;
    DigitFocusAnimState &state;

    uint16_t phase;
    Point startCorners[DigitFocus::CornerCount];
    Point endCorners[DigitFocus::CornerCount];

    void clearVisual(AnimationGate &gate);
    bool start(AnimationGate &gate);
    bool Tick(void);

    static bool TickThunk(void *ctx);
    static void CancelThunk(void *ctx);
};

}; // namespace HexCalc::AnimationEffects
