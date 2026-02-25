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
      // managers
      valueManager(formulaModel), formulaPaginator(eventBus, valueManager) {
    eventBus.Subscribe(config);
    // must subscribe before subscribing formulaModel
    eventBus.Subscribe(formulaPaginator);
    eventBus.Subscribe(formulaModel);
}

void
ViewModel::DispatchEvents(void) {
    eventBus.DispatchPending();
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

NumberWidth
ValueManager::GetNumberWidth(void) const {
    return config.Width();
}

NumberSign
ValueManager::GetNumberSign(void) const {
    return config.Sign();
}

NumberBase
ValueManager::GetNumberBase(void) const {
    return config.Base();
}

void
FormulaPaginator::notifyFormulaUpdate(void) {
    eventBus.Post(Event{
        0,
        EventType::FormulaUpdatedEvent,
    });
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
FormulaPaginator::formulaInsertOp(OperatorType op) {
    // If the first input is an operator, insert the current number as
    // the left operand. For example, if the user inputs '+' first, it
    // will be treated as '0 +'.
    if (formulaQueue.Empty()) {
        formulaInsertDigits();
    }
    // Insert a space if '1' + '+' -> '1 +'
    if (formulaState == InputDigit) {
        formulaQueue.Enqueue(Glyph(FontEmpty));
    }
    // Insert operator glyph
    if (op == OperatorType::Modulo) {
        // Insert 'mod' for modulo operator
        formulaQueue.Enqueue(Glyph(Font6x8M));
        formulaQueue.Enqueue(Glyph(Font6x8O));
        formulaQueue.Enqueue(Glyph(Font6x8D));
    } else {
        formulaQueue.Enqueue(OpGlyph(op));
    }
    // Update formula glyphs from the queue
    Glyph glyph;
    while (formulaQueue.Dequeue(glyph)) {
        formulaGlyphs.Insert(glyph);
    }
    notifyFormulaUpdate();
    if ((op != OperatorType::LeftBracket) || (op != OperatorType::Equal)) {
        formulaState = InputOp;
    } else {
        formulaState = InputDigit;
    }
}

void
FormulaPaginator::formulaInsertDigits() {
    if (formulaState == InputOp) {
        // Insert a space if '1 /' + '2' -> '1 / 2'
        formulaQueue.Enqueue(Glyph(FontEmpty));
    }
    auto base = vm.GetNumberBase();
    auto digits = vm.GetValueDigits<MaxDisplayDigits>(base);
    // reverse insert digits into the queue
    for (size_t i = digits.size; i > 0; i--) {
        formulaQueue.Enqueue(DigitGlyph(digits[i - 1]));
    }
    formulaState = InputDigit;
}

EventResult
FormulaPaginator::HandleEvent(const Event &e) {
    if (e.type == InputEvent) {
        if (formulaState == Evaluated) {
            // if the previous formula is evaluated, start a new formula when
            // inputting a digit or an operator
            formulaGlyphs.Clear();
        }
        // Update formula glyphs
        InputEventData inputData(e.data);
        if (inputData.isOp) {
            formulaInsertOp(inputData.data.op);
        } else {
            // do nothing
        }

        return Consumed;
    } else if (e.type == ClearEvent) {
        if (formulaQueue.Empty()) {
            formulaGlyphs.Clear();
        } else {
            formulaQueue.Clear();
        }

        notifyFormulaUpdate();

        return Consumed;
    } else if (e.type == EvaluateEvent) {
        if (formulaState == Evaluated) {
            // if the previous formula is evaluated, start a new formula when
            // inputting a digit or an operator
            formulaGlyphs.Clear();
        }
        if (formulaState == InputOp) {
            // if the last input is an operator, append the current number as
            // the right operand before evaluation. For example, '1 +' will be
            // treated as '1 + 0'.
            formulaInsertDigits();
        }
        formulaInsertOp(OperatorType::Equal);
        notifyFormulaUpdate();

        formulaState = Evaluated;

        return Consumed;
    }

    return Skipped;
}
