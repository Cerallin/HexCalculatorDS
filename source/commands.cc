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
        InputEventData(InputNum0, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber1(void) {
    debugf("Key 1 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum1, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber2(void) {
    debugf("Key 2 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum2, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber3(void) {
    debugf("Key 3 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum3, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber4(void) {
    debugf("Key 4 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum4, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber5(void) {
    debugf("Key 5 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum5, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber6(void) {
    debugf("Key 6 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum6, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber7(void) {
    debugf("Key 7 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum7, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber8(void) {
    debugf("Key 8 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum8, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumber9(void) {
    debugf("Key 9 pressed\n");
    bus.Post(Event{
        InputEventData(InputNum9, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberA(void) {
    debugf("Key A pressed\n");
    bus.Post(Event{
        InputEventData(InputNumA, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberB(void) {
    debugf("Key B pressed\n");
    bus.Post(Event{
        InputEventData(InputNumB, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberC(void) {
    debugf("Key C pressed\n");
    bus.Post(Event{
        InputEventData(InputNumC, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberD(void) {
    debugf("Key D pressed\n");
    bus.Post(Event{
        InputEventData(InputNumD, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberE(void) {
    debugf("Key E pressed\n");
    bus.Post(Event{
        InputEventData(InputNumE, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputNumberF(void) {
    debugf("Key F pressed\n");
    bus.Post(Event{
        InputEventData(InputNumF, false).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorPlus(void) {
    debugf("Key + pressed\n");
    bus.Post(Event{
        InputEventData(InputOpPlus, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMinus(void) {
    debugf("Key - pressed\n");
    bus.Post(Event{
        InputEventData(InputOpMinus, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorMultiply(void) {
    debugf("Key * pressed\n");
    bus.Post(Event{
        InputEventData(InputOpMultiply, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorDivide(void) {
    debugf("Key / pressed\n");
    bus.Post(Event{
        InputEventData(InputOpDivide, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorAnd(void) {
    debugf("Key & pressed\n");
    bus.Post(Event{
        InputEventData(InputOpAnd, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorOr(void) {
    debugf("Key | pressed\n");
    bus.Post(Event{
        InputEventData(InputOpOr, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorModulo(void) {
    debugf("Key %% pressed\n");
    bus.Post(Event{
        InputEventData(InputOpModulo, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorLShift(void) {
    debugf("Key << pressed\n");
    bus.Post(Event{
        InputEventData(InputOpLShift, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRShift(void) {
    debugf("Key >> pressed\n");
    bus.Post(Event{
        InputEventData(InputOpRShift, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorBackspace(void) {
    debugf("Key Backspace pressed\n");
    bus.Post(Event{
        InputEventData(InputOpBackspace, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorNegate(void) {
    debugf("Key +/- pressed\n");
    bus.Post(Event{
        InputEventData(InputOpNegate, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorNot(void) {
    debugf("Key ~ pressed\n");
    bus.Post(Event{
        InputEventData(InputOpBitwiseNot, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::Evaluate(void) {
    debugf("Key = pressed\n");
    bus.Post(Event{
        0,
        EventType::FormulaEvaluateEvent,
    });
}

void
Commands::InputOperatorLBrac(void) {
    debugf("Key ( pressed\n");
    bus.Post(Event{
        InputEventData(InputOpLBrac, true).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperatorRBrac(void) {
    debugf("Key ) pressed\n");
    bus.Post(Event{
        InputEventData(InputOpRBrac, true).ToInt(),
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
