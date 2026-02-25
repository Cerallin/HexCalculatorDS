/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "commands.h"
#include "config.h"
#include "model.h"
#include "number.h"

using namespace HexCalc;

void
Commands::InputNumber0(void) {
    debugf("Key 0 pressed\n");
    bus.Post(Event{
        InputEventData(Digit0).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber1(void) {
    debugf("Key 1 pressed\n");
    bus.Post(Event{
        InputEventData(Digit1).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber2(void) {
    debugf("Key 2 pressed\n");
    bus.Post(Event{
        InputEventData(Digit2).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber3(void) {
    debugf("Key 3 pressed\n");
    bus.Post(Event{
        InputEventData(Digit3).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber4(void) {
    debugf("Key 4 pressed\n");
    bus.Post(Event{
        InputEventData(Digit4).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber5(void) {
    debugf("Key 5 pressed\n");
    bus.Post(Event{
        InputEventData(Digit5).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber6(void) {
    debugf("Key 6 pressed\n");
    bus.Post(Event{
        InputEventData(Digit6).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber7(void) {
    debugf("Key 7 pressed\n");
    bus.Post(Event{
        InputEventData(Digit7).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber8(void) {
    debugf("Key 8 pressed\n");
    bus.Post(Event{
        InputEventData(Digit8).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber9(void) {
    debugf("Key 9 pressed\n");
    bus.Post(Event{
        InputEventData(Digit9).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberA(void) {
    debugf("Key A pressed\n");
    bus.Post(Event{
        InputEventData(DigitA).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberB(void) {
    debugf("Key B pressed\n");
    bus.Post(Event{
        InputEventData(DigitB).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberC(void) {
    debugf("Key C pressed\n");
    bus.Post(Event{
        InputEventData(DigitC).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberD(void) {
    debugf("Key D pressed\n");
    bus.Post(Event{
        InputEventData(DigitD).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberE(void) {
    debugf("Key E pressed\n");
    bus.Post(Event{
        InputEventData(DigitE).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberF(void) {
    debugf("Key F pressed\n");
    bus.Post(Event{
        InputEventData(DigitF).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorPlus(void) {
    debugf("Key + pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Plus).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMinus(void) {
    debugf("Key - pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Minus).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMultiply(void) {
    debugf("Key * pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Multiply).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorDivide(void) {
    debugf("Key / pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Divide).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorAnd(void) {
    debugf("Key & pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::BitwiseAnd).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorOr(void) {
    debugf("Key | pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::BitwiseOr).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorModulo(void) {
    debugf("Key %% pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Modulo).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorLShift(void) {
    debugf("Key << pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::LeftShift).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRShift(void) {
    debugf("Key >> pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::RightShift).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorBackspace(void) {
    debugf("Key Backspace pressed\n");
    bus.Post(Event{
        0,
        EventType::BackspaceEvent,
    });
}

void
Commands::InputOperatorNegate(void) {
    debugf("Key +/- pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::Negate).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorNot(void) {
    debugf("Key ~ pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::BitwiseNot).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::Evaluate(void) {
    debugf("Key = pressed\n");
    bus.Post(Event{
        0,
        EventType::EvaluateEvent,
    });
}

void
Commands::InputOperatorLBrac(void) {
    debugf("Key ( pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::LeftBracket).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRBrac(void) {
    debugf("Key ) pressed\n");
    bus.Post(Event{
        InputEventData(OperatorType::RightBracket).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::SwitchBaseUpper(void) {
    auto numberBase = config.Base();
    switch (numberBase) {
    case NumberBase::Binary:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Octal),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Octal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Decimal),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Decimal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Hexadecimal),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Hexadecimal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Binary),
                       EventType::UpdateBaseEvent});
        break;
    default:
        // should never reach here
        break;
    }
}

void
Commands::SwitchBaseLower(void) {
    auto numberBase = config.Base();
    switch (numberBase) {
    case NumberBase::Binary:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Hexadecimal),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Octal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Binary),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Decimal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Octal),
                       EventType::UpdateBaseEvent});
        break;
    case NumberBase::Hexadecimal:
        bus.Post(Event{static_cast<EventDataType>(NumberBase::Decimal),
                       EventType::UpdateBaseEvent});
        break;
    default:
        // should never reach here
        break;
    }
}

void
Commands::SwitchWidthUpper(void) {
    debugf("Switch width upper triggered\n");
    auto numberWidth = config.Width();
    switch (numberWidth) {
    case NumberWidth::Byte:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::Word),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::Word:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::DWord),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::DWord:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::QWord),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::QWord:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::Byte),
                       EventType::UpdateWidthEvent});
        break;
    default:
        // should never reach here
        break;
    }
}

void
Commands::SwitchWidthLower(void) {
    debugf("Switch width lower triggered\n");
    auto numberWidth = config.Width();
    switch (numberWidth) {
    case NumberWidth::Byte:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::QWord),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::Word:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::Byte),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::DWord:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::Word),
                       EventType::UpdateWidthEvent});
        break;
    case NumberWidth::QWord:
        bus.Post(Event{static_cast<EventDataType>(NumberWidth::DWord),
                       EventType::UpdateWidthEvent});
        break;
    default:
        // should never reach here
        break;
    }
}

void
Commands::ToggleSign(void) {
    debugf("Toggle sign triggered\n");
    auto numberSign = config.Sign();
    switch (numberSign) {
    case NumberSign::Unsigned:
        bus.Post(Event{static_cast<EventDataType>(NumberSign::Signed),
                       EventType::UpdateSignEvent});
        break;
    case NumberSign::Signed:
        bus.Post(Event{static_cast<EventDataType>(NumberSign::Unsigned),
                       EventType::UpdateSignEvent});
        break;
    default:
        // should never reach here
        break;
    }
}

void
Commands::Clear(void) {
    debugf("Clear triggered\n");
    bus.Post(Event{
        0,
        EventType::ClearEvent,
    });
}
