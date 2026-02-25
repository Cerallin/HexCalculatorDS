/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "viewmodel.h"
#include "config.h"
#include "event.h"
#include "input.h"

using namespace HexCalc;

ViewModel::ViewModel(void)
    : // event bus
      eventBus(),
      // commands
      commands(eventBus),
      // models
      formulaModel(eventBus),
      // input
      keyInputHandler(commands),
      // cache
      formulaGlyphs(), lastGlyphWasOperator(false) {
    eventBus.Subscribe(config);
    eventBus.Subscribe(formulaModel);
    eventBus.Subscribe(*this);
}

void
ViewModel::DispatchEvents(void) {
    eventBus.DispatchPending();
}

void
ViewModel::notifyFormulaUpdate(void) {
    eventBus.Post(Event{
        0,
        EventType::FormulaUpdatedEvent,
    });
}

bool
ViewModel::handleKeyInputs(void) {
    auto keyInput = ReadKeyInput();
    if (keyInput.Active()) {
        return keyInputHandler.Handle(keyInput);
    }

    return false;
}

bool
ViewModel::handleTouchScreen(void) {
    auto touchInput = ReadTouchInput();
    if (!touchInput.Active()) {
        return false;
    }

    // Get the touch point and post a TouchScreenEvent with the point data
    Point touchPoint = touchInput.point;
    eventBus.Post(Event{
        touchPoint.ToInt(),
        EventType::TouchScreenEvent,
    });

    return true;
}

void
ViewModel::HandleInputs(void) {
    // First try to handle key inputs,
    // if no keys are pressed, try to handle touch screen inputs.
    if (!handleKeyInputs()) {
        handleTouchScreen();
    }
}

static constexpr Glyph
OpGlyph(OperatorType op) {
    switch (op) {
    case OperatorType::Equal: // =
        return Glyph(Font6x8Equal);
    case OperatorType::LeftBracket: // (
        return Glyph(Font6x8LBrac);
    case OperatorType::RightBracket: // )
        return Glyph(Font6x8RBrac);
    case OperatorType::LeftShift: // <<
        return Glyph(Font6x8LShift);
    case OperatorType::RightShift: // >>
        return Glyph(Font6x8RShift);
    case OperatorType::BitwiseAnd: // &
        return Glyph(Font6x8And);
    case OperatorType::BitwiseOr: // |
        return Glyph(Font6x8Or);
    case OperatorType::Multiply: // *
        return Glyph(Font6x8Multiply);
    case OperatorType::Divide: // /
        return Glyph(Font6x8Divide);
    case OperatorType::Plus: // +
        return Glyph(Font6x8Plus);
    case OperatorType::Minus: // -
        return Glyph(Font6x8Minus);
    // Negate and bitwise not are not supported in the formula view
    // Modulo will use 3 glyphs 'mod'
    default:
        return InvalidGlyph;
    }
}

static constexpr Glyph
DigitGlyph(Digit digit) {
    switch (digit) {
    case Digit0:
        return Glyph(Font6x8Zero);
    case Digit1:
        return Glyph(Font6x8One);
    case Digit2:
        return Glyph(Font6x8Two);
    case Digit3:
        return Glyph(Font6x8Three);
    case Digit4:
        return Glyph(Font6x8Four);
    case Digit5:
        return Glyph(Font6x8Five);
    case Digit6:
        return Glyph(Font6x8Six);
    case Digit7:
        return Glyph(Font6x8Seven);
    case Digit8:
        return Glyph(Font6x8Eight);
    case Digit9:
        return Glyph(Font6x8Nine);
    case DigitA:
        return Glyph(Font6x8A);
    case DigitB:
        return Glyph(Font6x8B);
    case DigitC:
        return Glyph(Font6x8C);
    case DigitD:
        return Glyph(Font6x8D);
    case DigitE:
        return Glyph(Font6x8E);
    case DigitF:
        return Glyph(Font6x8F);
    default:
        return InvalidGlyph;
    }
}

void
ViewModel::formulaInsertOp(OperatorType op) {
    if (!lastGlyphWasOperator) {
        // Insert a space if '1' + '+' -> '1 +'
        formulaGlyphQueue.Enqueue(Glyph(FontEmpty));
    }
    if (op == OperatorType::Modulo) {
        // Insert 'mod' for modulo operator
        formulaGlyphQueue.Enqueue(Glyph(Font6x8M));
        formulaGlyphQueue.Enqueue(Glyph(Font6x8O));
        formulaGlyphQueue.Enqueue(Glyph(Font6x8D));
    } else {
        formulaGlyphQueue.Enqueue(OpGlyph(op));
    }
    Glyph glyph;
    while (formulaGlyphQueue.Dequeue(glyph)) {
        formulaGlyphs.Insert(glyph);
    }
    notifyFormulaUpdate();
    lastGlyphWasOperator = (op != OperatorType::LeftBracket);
}

void
ViewModel::formulaInsertDigit(Digit digit) {
    if (lastGlyphWasOperator) {
        // Insert a space if '1 /' + '2' -> '1 / 2'
        formulaGlyphQueue.Enqueue(Glyph(FontEmpty));
    }
    formulaGlyphQueue.Enqueue(DigitGlyph(digit));
    lastGlyphWasOperator = false;
}

EventResult
ViewModel::HandleEvent(const Event &e) {
    if (e.type == InputEvent) {
        // Update formula glyphs
        InputEventData inputData(e.data);
        if (inputData.isOp) {
            formulaInsertOp(inputData.data.op);
        } else {
            formulaInsertDigit(inputData.data.digit);
        }

        return Consumed;
    } else if (e.type == ClearEvent) {
        formulaGlyphs.Clear();

        Glyph glyph;
        while (formulaGlyphQueue.Dequeue(glyph))
            ;

        notifyFormulaUpdate();

        return Consumed;
    } else if (e.type == EvaluateEvent) {
        formulaInsertOp(OperatorType::Equal);
        notifyFormulaUpdate();

        return Consumed;
    }

    return Skipped;
}

NumberWidth
ViewModel::GetNumberWidth(void) const {
    return config.Width();
}

NumberSign
ViewModel::GetNumberSign(void) const {
    return config.Sign();
}

NumberBase
ViewModel::GetNumberBase(void) const {
    return config.Base();
}
