/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationeffects.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;

IndicatorBarSlide::IndicatorBarSlide(IndicatorView &view)
    : view(view), targetBase(view.GetCurrentBase()), fromY(0), toY(0),
      phase(0) {}

bool
IndicatorBarSlide::TryHandle(const Event &e, AnimationGate &gate) {
    if (e.type != EventType::UpdateBaseEvent) {
        return false;
    }

    const NumberBase nextBase = view.GetVM().GetNumberBase();
    if (nextBase == view.GetCurrentBase() && !active) {
        return false;
    }

    if (active) {
        gate.Cancel(AnimationChannel::NonBlocking);
    }

    fromY = view.GetBarY();
    toY = view.IndicatorY(nextBase);
    targetBase = nextBase;

    if (fromY == toY) {
        snapToTarget();
        return true;
    }

    if (!start(gate)) {
        snapToTarget();
        return true;
    }

    return true;
}

bool
IndicatorBarSlide::SuppressesViewUpdate(void) const {
    return true;
}

void
IndicatorBarSlide::Cancel(void) {
    active = false;
    phase = 0;
}

bool
IndicatorBarSlide::start(AnimationGate &gate) {
    phase = 0;
    if (!gate.Start(AnimationChannel::NonBlocking,
                    Animation(this, &IndicatorBarSlide::TickThunk,
                              &IndicatorBarSlide::CancelThunk))) {
        active = false;
        return false;
    }
    active = true;
    return true;
}

void
IndicatorBarSlide::snapToTarget(void) {
    view.SetBarY(toY);
    view.SetCurrentBase(targetBase);
    active = false;
    phase = 0;
}

bool
IndicatorBarSlide::Tick(void) {
    if (!active) {
        return false;
    }

    phase++;
    if (phase >= MoveFrames) {
        snapToTarget();
        return false;
    }

    const uint16_t t256 = static_cast<uint16_t>(
        (static_cast<uint32_t>(phase) * 256) / MoveFrames);
    const uint16_t eased = EaseInOutCubicBezier(t256);
    view.SetBarY(static_cast<int16_t>(LerpInt(fromY, toY, eased)));
    return true;
}

bool
IndicatorBarSlide::TickThunk(void *ctx) {
    return static_cast<IndicatorBarSlide *>(ctx)->Tick();
}

void
IndicatorBarSlide::CancelThunk(void *ctx) {
    static_cast<IndicatorBarSlide *>(ctx)->Cancel();
}
