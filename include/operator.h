/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"
#include "number.h"

namespace HexCalc {

/**
 * @brief The operators supported by the calculator.
 *
 */
enum OperatorType : uint8_t {
    None,         //
    Equal,        // =
    LeftBracket,  // (
    RightBracket, // )
    LeftShift,    // <<
    RightShift,   // >>
    Modulo,       // %
    BitwiseAnd,   // &
    BitwiseOr,    // |
    Multiply,     // *
    Divide,       // /
    Plus,         // +
    Minus,        // -
    Negate,       // +/-
    BitwiseNot,   // ~
};

namespace Operator {

/**
 * @brief Lower value means higher precedence.
 *
 * @param op
 * @return constexpr int
 */
constexpr int
Precedence(OperatorType op) {
    switch (op) {
    case OperatorType::Equal:
        return 0;
    case OperatorType::LeftBracket:
    case OperatorType::RightBracket:
        return 1;

    case OperatorType::Multiply:
    case OperatorType::Divide:
    case OperatorType::Modulo:
        return 2;

    case OperatorType::Plus:
    case OperatorType::Minus:
        return 3;

    case OperatorType::LeftShift:
    case OperatorType::RightShift:
        return 4;

    case OperatorType::BitwiseAnd:
        return 5;

    case OperatorType::BitwiseOr:
        return 6;

    default:
        return 100;
    }
}

/**
 * @brief Compare the precedence of two operators. Return true if lhs has lower
 * precedence than rhs.
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
static constexpr bool
LowerThan(OperatorType lhs, OperatorType rhs) {
    return Precedence(lhs) >= Precedence(rhs);
}

/**
 * @brief Compare the precedence of two operators. Return true if lhs has higher
 * precedence than rhs.
 *
 * @param lhs
 * @param rhs
 * @return true
 * @return false
 */
static constexpr bool
HigherThan(OperatorType lhs, OperatorType rhs) {
    return Precedence(lhs) < Precedence(rhs);
}

/**
 * @brief Check if the operator is a unary operator.
 *
 * @param op The operator to check.
 * @return true if the operator is unary, false otherwise (is binary).
 */
constexpr bool
Unary(OperatorType op) {
    switch (op) {
    case Equal:        // =
    case LeftBracket:  // (
    case RightBracket: // )
    case Negate:       // +/-
    case BitwiseNot:   // ~
        return true;

    default:
        return false;
    }
}

NumberDataType LeftShift(NumberDataType a, NumberDataType b);
NumberDataType RightShift(NumberDataType a, NumberDataType b);
NumberDataType Modulo(NumberDataType a, NumberDataType b);
NumberDataType And(NumberDataType a, NumberDataType b);
NumberDataType Or(NumberDataType a, NumberDataType b);
NumberDataType Multiply(NumberDataType a, NumberDataType b);
NumberDataType Divide(NumberDataType a, NumberDataType b);
NumberDataType Plus(NumberDataType a, NumberDataType b);
NumberDataType Minus(NumberDataType a, NumberDataType b);

}; // namespace Operator

}; // namespace HexCalc
