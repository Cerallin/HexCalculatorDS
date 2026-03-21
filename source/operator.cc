/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "operator.h"
#include "common.h"
#include "config.h"
#include "number.h"

using namespace HexCalc;

/**
 * @brief Sign-extend a value to the specified width.
 *
 * @tparam T
 * @param v
 * @param w
 * @return constexpr T
 */
template <typename T>
static constexpr T
_signExtend(uint64_t v, NumberWidth w) {
    uint64_t mask = WidthMask(w);
    v &= mask;

    if constexpr (std::is_signed_v<T>) {
        uint64_t signBit = (mask + 1) >> 1;
        if (v & signBit) {
            v |= ~mask;
        }
    }
    return static_cast<T>(v);
}

template <typename T>
static HEXCALC_ARM_CODE constexpr uint64_t
_doOp(OperatorType op, T a, T b) {
    switch (op) {
    case OperatorType::Plus:
        return static_cast<uint64_t>(a + b);
    case OperatorType::Minus:
        return static_cast<uint64_t>(a - b);
    case OperatorType::Multiply:
        return static_cast<uint64_t>(a * b);
    case OperatorType::Divide:
        return static_cast<uint64_t>(b == 0 ? 0 : (a / b));
    case OperatorType::Modulo:
        return static_cast<uint64_t>(b == 0 ? 0 : (a % b));
    case OperatorType::BitwiseAnd:
        return static_cast<uint64_t>(a & b);
    case OperatorType::BitwiseOr:
        return static_cast<uint64_t>(a | b);
    case OperatorType::LeftShift:
        return static_cast<uint64_t>(a << b);
    case OperatorType::RightShift:
        return static_cast<uint64_t>(a >> b);
    default:
        return 0;
    }
}

/**
 * @brief Perform the specified operation on two numbers, taking into account
 * the current configuration for sign and width.
 *
 * @param op The operator type to perform.
 * @param a The first operand.
 * @param b The second operand.
 * @return NumberDataType The result of the operation, masked to fit the current
 * width.
 */
static NumberDataType
_operateCalc(OperatorType op, NumberDataType a, NumberDataType b) {
    auto sign = config.Sign();
    auto width = config.Width();

    uint64_t mask = WidthMask(width);

    a &= mask;
    b &= mask;

    uint64_t result = 0;

    if (sign == Signed) {
        switch (width) {
        case Byte: {
            int8_t aa = _signExtend<int8_t>(a, width);
            int8_t bb = _signExtend<int8_t>(b, width);
            result = _doOp(op, aa, bb);
            break;
        }
        case Word: {
            int16_t aa = _signExtend<int16_t>(a, width);
            int16_t bb = _signExtend<int16_t>(b, width);
            result = _doOp(op, aa, bb);
            break;
        }
        case DWord: {
            int32_t aa = _signExtend<int32_t>(a, width);
            int32_t bb = _signExtend<int32_t>(b, width);
            result = _doOp(op, aa, bb);
            break;
        }
        case QWord: {
            int64_t aa = _signExtend<int64_t>(a, width);
            int64_t bb = _signExtend<int64_t>(b, width);
            result = _doOp(op, aa, bb);
            break;
        }
        }

    } else {

        switch (width) {
        case Byte: {
            uint8_t aa = static_cast<uint8_t>(a);
            uint8_t bb = static_cast<uint8_t>(b);
            result = _doOp(op, aa, bb);
            break;
        }
        case Word: {
            uint16_t aa = static_cast<uint16_t>(a);
            uint16_t bb = static_cast<uint16_t>(b);
            result = _doOp(op, aa, bb);
            break;
        }
        case DWord: {
            uint32_t aa = static_cast<uint32_t>(a);
            uint32_t bb = static_cast<uint32_t>(b);
            result = _doOp(op, aa, bb);
            break;
        }
        case QWord: {
            uint64_t aa = static_cast<uint64_t>(a);
            uint64_t bb = static_cast<uint64_t>(b);
            result = _doOp(op, aa, bb);
            break;
        }
        }
    }

    // Mask the result to fit the current width
    return result & mask;
}

namespace HexCalc::Operator {

NumberDataType
LeftShift(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::LeftShift, a, b);
}

NumberDataType
RightShift(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::RightShift, a, b);
}

NumberDataType
Modulo(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::Modulo, a, b);
}

NumberDataType
And(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::BitwiseAnd, a, b);
}

NumberDataType
Or(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::BitwiseOr, a, b);
}

NumberDataType
Multiply(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::Multiply, a, b);
}

NumberDataType
Divide(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::Divide, a, b);
}

NumberDataType
Plus(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::Plus, a, b);
}

NumberDataType
Minus(NumberDataType a, NumberDataType b) {
    return _operateCalc(OperatorType::Minus, a, b);
}

}; // namespace HexCalc::Operator
