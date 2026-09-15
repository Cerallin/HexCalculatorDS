/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "commands.h"
#include "config.h"
#include "input.h"
#include "model.h"
#include "number.h"

using namespace HexCalc;

void
Commands::InputNumber(Digit digit) {
    debugf("Key %X pressed\n", static_cast<int>(digit));
    bus.Post(Event{
        InputEventData(digit).ToInt(),
        EventType::InputEvent,
    });
}

void
Commands::InputOperator(OperatorType op) {
    debugf("Key operator %d pressed\n", static_cast<int>(op));
    bus.Post(Event{
        InputEventData(op).ToInt(),
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
Commands::Evaluate(void) {
    debugf("Key = pressed\n");
    bus.Post(Event{
        0,
        EventType::EvaluateEvent,
    });
}

void
Commands::InputPrevious(void) {
    debugf("Key Previous pressed\n");
    bus.Post(Event{
        0,
        EventType::PreviousTouchEvent,
    });
}

void
Commands::FlipBit(int index) {
    debugf("Flip digit %d pressed\n", index);
    bus.Post(Event{
        index,
        EventType::FlipBitEvent,
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
Commands::InputLeftShift(void) {
    bus.Post(Event{static_cast<EventDataType>(LeftShift),
                   EventType::NumberShiftEvent});
}

void
Commands::InputRightShift(void) {
    bus.Post(Event{static_cast<EventDataType>(RightShift),
                   EventType::NumberShiftEvent});
}

void
Commands::SwitchShiftModeLeft(void) {
    debugf("Switch shift mode left <- triggered\n");

    bus.Post(Event{DirLeft, EventType::SwitchShiftModeEvent});
}

void
Commands::SwitchShiftModeRight(void) {
    debugf("Switch shift mode right -> triggered\n");

    bus.Post(Event{DirRight, EventType::SwitchShiftModeEvent});
}

void
Commands::Clear(void) {
    debugf("Clear triggered\n");
    bus.Post(Event{
        0,
        EventType::ClearEvent,
    });
}

void
Commands::SwitchFormulaPageLeft(void) {
    debugf("Switch formula page left triggered\n");
    bus.Post(Event{
        DirLeft,
        EventType::SwitchFormulaPageEvent,
    });
}

void
Commands::SwitchFormulaPageRight(void) {
    debugf("Switch formula page right triggered\n");
    bus.Post(Event{
        DirRight,
        EventType::SwitchFormulaPageEvent,
    });
}

void
Commands::MoveFocusUp(void) {
    debugf("Move focus up triggered\n");
    bus.Post(Event{
        Direction::DirUp,
        EventType::MoveFocusEvent,
    });
}

void
Commands::MoveFocusDown(void) {
    debugf("Move focus down triggered\n");
    bus.Post(Event{
        Direction::DirDown,
        EventType::MoveFocusEvent,
    });
}

void
Commands::MoveFocusLeft(void) {
    debugf("Move focus left triggered\n");
    bus.Post(Event{
        Direction::DirLeft,
        EventType::MoveFocusEvent,
    });
}

void
Commands::MoveFocusRight(void) {
    debugf("Move focus right triggered\n");
    bus.Post(Event{
        Direction::DirRight,
        EventType::MoveFocusEvent,
    });
}
