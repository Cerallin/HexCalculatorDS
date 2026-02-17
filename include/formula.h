/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#pragma once

#include "common.h"

#include "number.h"
#include "operator.h"
#include "structure.h"
#include "traits.h"

namespace HexCalc {

/**
 * @brief A node in the formula tree, either a number or an operator.
 */
class FormulaData {
  public:
    explicit FormulaData(NumberDataType value)
        : value{.number = value}, isOperator(false), isMatched(false) {}
    explicit FormulaData(OperatorType op)
        : value{.op = op}, isOperator(true), isMatched(false) {}

    bool
    IsOperator() const {
        return isOperator;
    }

    bool
    IsNumber() const {
        return !isOperator;
    }

  private:
    union FormulaValue {
        NumberDataType number;
        OperatorType op;
    } value;
    // Whether this node is an operator or a number
    bool isOperator;
    // Only for left bracket: whether it has a matching right bracket
    bool isMatched;
};

class FormulaTree : private NonCopyable {
  public:
    explicit FormulaTree(void) : size(0), nodes{} { Clear(); }

    bool Input(const FormulaData &data);

    void Clear(void);

    bool Evaluate(void);

    static constexpr size_t MaxSize = 64;

  private:
    using FormulaTreeNode = TreeNode<FormulaData>;

    // TODO Number of brackets also needs to be limited.

    /**
     * @brief The array of nodes to avoid allocating memory on the heap, which
     * is not recommended on Nintendo DS. So we can have at most MaxSize nodes
     * in the formula tree, which should be enough for a simple calculator.
     */
    FormulaTreeNode nodes[MaxSize];
    size_t size;

    constexpr FormulaTreeNode &
    root() {
        return nodes[0];
    }
};

}; // namespace HexCalc
