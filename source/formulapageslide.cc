/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "formulapageslide.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

namespace {

constexpr int16_t kStepPx = 12;
constexpr int16_t kCharWidth = GlyphArray6x8<0>::CharWidth;

} // namespace

FormulaPageSlide::FormulaPageSlide(FormulaView &view)
    : view(view), display(view.GetDisplay()), vm(view.GetVM()),
      area(view.GetArea()), active(false), dir(DirLeft), offsetPx(0),
      distancePx(0), baseX(0), lastStart(0), strip() {}

bool
FormulaPageSlide::TryHandle(const Event &e, AnimationGate &gate) {
    if (e.type == EventType::FormulaUpdatedEvent) {
        if (!active) {
            lastStart = vm.GetWindowStart();
        }
        return false;
    }

    if (e.type != EventType::SwitchFormulaPageEvent) {
        return false;
    }

    const size_t currentStart = vm.GetWindowStart();
    if (currentStart == lastStart) {
        return false;
    }

    const auto from = vm.GetFormulaPageAt(lastStart);
    const auto to = vm.GetFormulaPageAt(currentStart);

    const size_t fromEnd = from.StartIndex + from.Glyphs.Size();
    const size_t toEnd = to.StartIndex + to.Glyphs.Size();
    const size_t stripStart =
        from.StartIndex < to.StartIndex ? from.StartIndex : to.StartIndex;
    const size_t stripEnd = fromEnd > toEnd ? fromEnd : toEnd;

    strip = vm.GetFormulaSlice(stripStart, stripEnd - stripStart);

    Area6x8 charArea(area);
    const int16_t fromPageStartX = static_cast<int16_t>(
        area.x + (charArea.w - from.Glyphs.Size()) * kCharWidth);
    const size_t fromInStrip = from.StartIndex - stripStart;
    baseX = static_cast<int16_t>(fromPageStartX - fromInStrip * kCharWidth);

    const size_t startDelta = from.StartIndex > to.StartIndex
                                  ? from.StartIndex - to.StartIndex
                                  : to.StartIndex - from.StartIndex;
    distancePx = static_cast<int16_t>(startDelta * kCharWidth);

    dir = static_cast<Direction>(e.data);
    offsetPx = 0;
    active = true;

    if (!gate.Start(AnimationChannel::Blocking,
                    Animation(this, &FormulaPageSlide::TickThunk))) {
        active = false;
        lastStart = currentStart;
        view.Invalidate();
        return false;
    }

    return true;
}

bool
FormulaPageSlide::Suppress(const Event &e) const {
    return active && (e.type == EventType::FormulaUpdatedEvent);
}

bool
FormulaPageSlide::IsActive(void) const {
    return active;
}

void
FormulaPageSlide::AfterHandle(const Event &, AnimationGate &) {
    // do nothing
}

void
FormulaPageSlide::Cancel(void) {
    // Blocking animations are not cancellable
}

bool
FormulaPageSlide::SuppressesViewUpdate(void) const {
    return true;
}

bool
FormulaPageSlide::TickThunk(void *ctx) {
    return static_cast<FormulaPageSlide *>(ctx)->Tick();
}

bool
FormulaPageSlide::Tick(void) {
    DrawFrame();

    offsetPx = static_cast<int16_t>(offsetPx + kStepPx);
    if (offsetPx >= distancePx) {
        active = false;
        lastStart = vm.GetWindowStart();
        view.Invalidate();
        return false;
    }

    return true;
}

void
FormulaPageSlide::DrawFrame(void) const {
    Point start(area.x, area.y);
    display.ClearLineBand(start, area.w);

    int16_t scroll;
    if (dir == DirLeft) {
        scroll = offsetPx;
    } else {
        scroll = static_cast<int16_t>(-offsetPx);
    }
    DrawStrip(scroll);
}

void
FormulaPageSlide::DrawStrip(int16_t xOffset) const {
    int16_t x = static_cast<int16_t>(baseX + xOffset);
    const int16_t left = area.x;
    const int16_t right = static_cast<int16_t>(area.x + area.w);

    for (const auto &glyph : strip) {
        if (x >= left && static_cast<int16_t>(x + kCharWidth) <= right) {
            display.PrintGlyph(x, area.y, glyph);
        }
        x = static_cast<int16_t>(x + kCharWidth);
    }
}
