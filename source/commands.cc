/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "commands.h"
#include "config.h"
#include "number.h"

using namespace HexCalc;

void
Commands::InputNumber0(void) {
    debugf("Key 0 pressed\n");
    bus.Post(Event{
        InputNum0,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber1(void) {
    debugf("Key 1 pressed\n");
    bus.Post(Event{
        InputNum1,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber2(void) {
    debugf("Key 2 pressed\n");
    bus.Post(Event{
        InputNum2,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber3(void) {
    debugf("Key 3 pressed\n");
    bus.Post(Event{
        InputNum3,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber4(void) {
    debugf("Key 4 pressed\n");
    bus.Post(Event{
        InputNum4,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber5(void) {
    debugf("Key 5 pressed\n");
    bus.Post(Event{
        InputNum5,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber6(void) {
    debugf("Key 6 pressed\n");
    bus.Post(Event{
        InputNum6,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber7(void) {
    debugf("Key 7 pressed\n");
    bus.Post(Event{
        InputNum7,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber8(void) {
    debugf("Key 8 pressed\n");
    bus.Post(Event{
        InputNum8,
        EventType::InputEvent,
    });
}

void
Commands::InputNumber9(void) {
    debugf("Key 9 pressed\n");
    bus.Post(Event{
        InputNum9,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberA(void) {
    debugf("Key A pressed\n");
    bus.Post(Event{
        InputNumA,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberB(void) {
    debugf("Key B pressed\n");
    bus.Post(Event{
        InputNumB,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberC(void) {
    debugf("Key C pressed\n");
    bus.Post(Event{
        InputNumC,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberD(void) {
    debugf("Key D pressed\n");
    bus.Post(Event{
        InputNumD,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberE(void) {
    debugf("Key E pressed\n");
    bus.Post(Event{
        InputNumE,
        EventType::InputEvent,
    });
}

void
Commands::InputNumberF(void) {
    debugf("Key F pressed\n");
    bus.Post(Event{
        InputNumF,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorPlus(void) {
    debugf("Key + pressed\n");
    bus.Post(Event{
        InputOpPlus,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMinus(void) {
    debugf("Key - pressed\n");
    bus.Post(Event{
        InputOpMinus,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMultiply(void) {
    debugf("Key * pressed\n");
    bus.Post(Event{
        InputOpMultiply,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorDivide(void) {
    debugf("Key / pressed\n");
    bus.Post(Event{
        InputOpDivide,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorAnd(void) {
    debugf("Key & pressed\n");
    bus.Post(Event{
        InputOpAnd,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorOr(void) {
    debugf("Key | pressed\n");
    bus.Post(Event{
        InputOpOr,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorModulo(void) {
    debugf("Key %% pressed\n");
    bus.Post(Event{
        InputOpModulo,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorLShift(void) {
    debugf("Key << pressed\n");
    bus.Post(Event{
        InputOpLShift,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRShift(void) {
    debugf("Key >> pressed\n");
    bus.Post(Event{
        InputOpRShift,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorBackspace(void) {
    debugf("Key Backspace pressed\n");
    bus.Post(Event{
        InputOpBackspace,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorNegate(void) {
    debugf("Key +/- pressed\n");
    bus.Post(Event{
        InputOpNegate,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorNot(void) {
    debugf("Key ~ pressed\n");
    bus.Post(Event{
        InputOpBitwiseNot,
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
        InputOpLBrac,
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRBrac(void) {
    debugf("Key ) pressed\n");
    bus.Post(Event{
        InputOpRBrac,
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
