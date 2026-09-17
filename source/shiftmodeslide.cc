/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "animationeffects.h"
#include "viewhost.h"

using namespace HexCalc;
using namespace HexCalc::AnimationEffects;
using SMM = HexCalc::ShiftModeManager;

namespace {

constexpr int16_t kStepPx = SMM::TileWidthPx;
constexpr int16_t kDistancePx = static_cast<int16_t>(SMM::TextWidth * kStepPx);

} // namespace

ShiftModeSlide::ShiftModeSlide(EditorView &editorView)
    : editorView(editorView), vm(editorView.GetVM()),
      shiftModes(editorView.GetShiftModeManager()), dir(DirLeft),
      fromMode(ArithmeticMode), toMode(ArithmeticMode),
      lastMode(vm.GetShiftMode()), offsetPx(0), distancePx(kDistancePx) {}

bool
ShiftModeSlide::TryHandle(const Event &e, AnimationGate &gate) {
    if (e.type != EventType::SwitchShiftModeEvent) {
        return false;
    }

    toMode = vm.GetShiftMode();
    fromMode = lastMode;
    if (fromMode == toMode) {
        return false;
    }

    dir = static_cast<Direction>(e.data);
    offsetPx = 0;
    distancePx = kDistancePx;
    active = true;

    if (!gate.Start(AnimationChannel::Blocking,
                    Animation(this, &ShiftModeSlide::TickThunk))) {
        active = false;
        lastMode = toMode;
        editorView.Invalidate();
        return false;
    }

    return true;
}

bool
ShiftModeSlide::SuppressesViewUpdate(void) const {
    return true;
}

bool
ShiftModeSlide::TickThunk(void *ctx) {
    return static_cast<ShiftModeSlide *>(ctx)->Tick();
}

bool
ShiftModeSlide::Tick(void) {
    DrawFrame();

    offsetPx = static_cast<int16_t>(offsetPx + kStepPx);
    if (offsetPx >= distancePx) {
        active = false;
        lastMode = toMode;
        editorView.Invalidate();
        return false;
    }

    return true;
}

void
ShiftModeSlide::DrawFrame(void) const {
    const auto &fromMap = SMM::ShiftModeTextTileMap(fromMode);
    const auto &toMap = SMM::ShiftModeTextTileMap(toMode);
    const int offsetCols = offsetPx / kStepPx;

    for (int col = 0; col < SMM::TextWidth; col++) {
        int stripCol;
        if (dir == DirLeft) {
            stripCol = col + offsetCols;
        } else {
            stripCol = col - offsetCols;
        }

        for (int row = 0; row < SMM::TextHeight; row++) {
            int mapIndex = 0;
            if (stripCol >= 0 && stripCol < SMM::TextWidth) {
                mapIndex = fromMap[row][stripCol];
            } else if (stripCol >= SMM::TextWidth &&
                       stripCol < ShiftModeManager::TextWidth * 2) {
                mapIndex = toMap[row][stripCol - SMM::TextWidth];
            } else if (stripCol < 0 && stripCol >= -SMM::TextWidth) {
                mapIndex = toMap[row][stripCol + SMM::TextWidth];
            }
            shiftModes.PutMapTile(col, row, mapIndex);
        }
    }
}
