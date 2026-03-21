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

ViewModel::ViewModel(EventBus &eventBus, Commands &commands)
    : // event bus
      eventBus(eventBus),
      // commands
      commands(commands),
      // models
      formulaModel(eventBus),
      // managers
      valueManager(formulaModel), formulaManager(eventBus, valueManager) {
    eventBus.Subscribe(config);
    // must subscribe before subscribing formulaModel
    eventBus.Subscribe(formulaManager);
    eventBus.Subscribe(formulaModel);
}

void
ViewModel::DispatchEvents(void) {
    eventBus.DispatchPending();
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
FormulaManager::notifyFormulaUpdate(void) {
    eventBus.Post(Event{
        0,
        EventType::FormulaUpdatedEvent,
    });
}

static constexpr Glyph
opGlyph(OperatorType op) {
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
digitGlyph(Digit digit) {
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
FormulaManager::formulaInsertOp(OperatorType op) {
    if (formulaState == Evaluated) {
        formulaGlyphs.Clear();
    } else {
        if (op == OperatorType::LeftBracket) {
            // Insert a multiplication operator if the user inputs '(' after a
            // digit, e.g. '2' + '(' -> '2 x ('
            if (formulaState == InputDigit) {
                formulaInsertOp(OperatorType::Multiply);
            }

            // Insert a space between last operator and the left bracket
            // e.g. '1 x' + '(' -> '1 x ('
            if (formulaState == InputOp) {
                formulaGlyphs.Insert(Glyph(FontEmpty));
            }
        }
    }
    // Insert a space if '1' + '+' -> '1 +'
    if (formulaState == InputDigit) {
        if (op != OperatorType::RightBracket) {
            formulaGlyphs.Insert(Glyph(FontEmpty));
        }
    }
    // Insert operator glyph
    if (op == OperatorType::Modulo) {
        // Insert 'mod' for modulo operator
        formulaGlyphs.Insert(Glyph(Font6x8M));
        formulaGlyphs.Insert(Glyph(Font6x8O));
        formulaGlyphs.Insert(Glyph(Font6x8D));
    } else {
        formulaGlyphs.Insert(opGlyph(op));
    }
    // Set formula state
    if (op == OperatorType::LeftBracket) {
        formulaState = InputBracket;
        leftBracketCount++;
    } else if (op == OperatorType::RightBracket) {
        formulaState = InputDigit;
        leftBracketCount--;
    } else {
        formulaState = InputOp;
    }
    notifyFormulaUpdate();
}

void
FormulaManager::formulaInsertDigits() {
    if (formulaState == Evaluated) {
        formulaGlyphs.Clear();
    }

    if (formulaState == InputOp) {
        // Insert a space if '1 /' + '2' -> '1 / 2'
        formulaGlyphs.Insert(Glyph(FontEmpty));
    }

    auto base = vm.GetNumberBase();
    auto sign = vm.GetNumberSign();
    auto width = vm.GetNumberWidth();

    Number number(currentNumber, width, sign);
    auto digits = number.Transcode<MaxDisplayDigits>(base);

    if (base == NumberBase::Decimal && sign == NumberSign::Signed) {
        // Insert a space if the number is negative
        // e.g. '1 +' + '-2' -> '1 + -2'
        if (digits.negative) {
            formulaGlyphs.Insert(Glyph(Font6x8Minus));
        }
    }
    // reverse insert digits into the queue
    for (size_t i = digits.size; i > 0; i--) {
        formulaGlyphs.Insert(digitGlyph(digits[i - 1]));
    }
    formulaState = InputDigit;
}

bool
FormulaManager::switchPage(Direction dir) {
    if (dir == DirRight) {
        if (currentPage > 1) {
            currentPage--;

            return true;
        }
    } else if (dir == DirLeft) {
        if ((currentPage * MaxPageGlyphs) < formulaGlyphs.Size()) {
            currentPage++;

            return true;
        }
    }

    return false;
}

void
FormulaManager::resetFormulaState(void) {
    formulaState = Evaluated;
    leftBracketCount = 0;
    currentPage = 1;
}

EventResult
FormulaManager::HandleEvent(const Event &e) {
    if (e.type == NumberAcceptEvent) {
        auto number = static_cast<uint32_t>(e.data);
        debugf("NumberAcceptEvent: 0x%X\n", number);
        if (!collectingNumber) {
            currentNumber =
                static_cast<NumberDataType>(number & WidthMask(DWord));
            collectingNumber = true;
        } else {
            currentNumber =
                (currentNumber << DWord) | static_cast<NumberDataType>(number);
            collectingNumber = false;
            formulaInsertDigits();
            notifyFormulaUpdate();
        }
    } else if (e.type == OperatorAcceptEvent) {
        auto op = static_cast<OperatorType>(e.data);
        debugf("OperatorAcceptEvent: %d\n", op);
        // insert ')'s before inserting '=' if there are unclosed left brackets,
        // e.g. '(1 + 0' + '=' -> '(1 + 0)'
        if (op == OperatorType::Equal) {
            while (leftBracketCount > 0) {
                // leftBracketCount will be decremented in formulaInsertOp when
                // inserting right bracket, so we do not need to decrement it
                // here.
                formulaInsertOp(OperatorType::RightBracket);
            }
        }
        formulaInsertOp(op);
        // must after '=' is inserted
        if (op == OperatorType::Equal) {
            formulaState = Evaluated;
        }
        // Always reset current page to 1 after operator is accepted since the
        // user may want to see the result
        currentPage = 1;
        notifyFormulaUpdate();
    } else if (e.type == EvaluateErrorEvent) {
        // Reset formula state to allow user to input new formula
        resetFormulaState();
        notifyFormulaUpdate();

        return Consumed;
    } else if (e.type == ClearEvent) {
        formulaGlyphs.Clear();
        resetFormulaState();
        notifyFormulaUpdate();

        return Consumed;
    } else if (e.type == SwitchFormulaPageEvent) {
        auto dir = static_cast<Direction>(e.data);
        if (switchPage(dir)) {
            notifyFormulaUpdate();

            return Consumed;
        }

        return Skipped;
    } else {
        return Skipped;
    }

    return Skipped;
}
