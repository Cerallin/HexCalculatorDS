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
    explicit FormulaData(NumberDataType number, bool shadow = false)
        : number(number), op(None), isOperator(false), isPaired(false),
          isShadow(shadow) {}
    explicit FormulaData(OperatorType op)
        : number(NumberZero), op(op), isOperator(true), isPaired(false),
          isShadow(false) {}

    constexpr bool
    IsOperator(void) const {
        return isOperator;
    }

    constexpr OperatorType
    GetOperator(void) const {
        return op;
    }

    constexpr bool
    IsNumber(void) const {
        return !isOperator;
    }

    constexpr NumberDataType
    GetNumber(void) const {
        return number;
    }

    void
    SetNumber(NumberDataType n) {
        assert(isOperator);
        number = n;
    }

    constexpr bool
    IsShadow(void) const {
        return isShadow;
    }

    constexpr bool
    Paired() const {
        return isPaired;
    }

    void
    SetPaired(bool Paired) {
        isPaired = Paired;
    }

  private:
    NumberDataType number;
    OperatorType op;
    // Whether this node is an operator or a number
    bool isOperator;
    // Only for left bracket: whether it has a matching right bracket
    bool isPaired;
    // Only for number type, if number is just a placeholder
    bool isShadow;
};

using FormulaTreeNode = TreeNode<FormulaData>;

class FormulaTree : private NonCopyable {
  public:
    explicit FormulaTree(void) : nodes{}, currentNode(&nodes[0]), size(0) {
        Clear();
    }

    bool Input(const FormulaData &data);

    void Clear(void);

    bool Evaluate(void);

    NumberDataType
    Result() const {
        return root.Get().GetNumber();
    }

    static constexpr size_t MaxSize = 64;

  private:
    // TODO Number of brackets also needs to be limited.

    /**
     * @brief The array of nodes to avoid allocating memory on the heap, which
     * is not recommended on Nintendo DS. So we can have at most MaxSize nodes
     * in the formula tree, which should be enough for a simple calculator.
     */
    FormulaTreeNode nodes[MaxSize];
    /**
     * @brief root node, will be enclosed by another equal operator node.
     */
    FormulaTreeNode root{FormulaData{OperatorType::Equal}};
    FormulaTreeNode *currentNode;
    size_t size;

    FormulaTreeNode &
    newNode() {
        auto node = &nodes[size++];
        node->Reset();

        return *node;
    }

    FormulaTreeNode *findNearestUnPairedLeftBracket();

    bool
    treeNodeIsCompleted(const FormulaTreeNode &node) {
        if (node.Get().IsNumber()) {
            return true;
        }
        if (node.Get().IsOperator()) {
            auto op = node.Get().GetOperator();
            if (Operator::Unary(op)) {
                return node.ChildCount() >= 1;
            } else {
                return node.ChildCount() >= 2;
            }
        }
        return false;
    }
};

}; // namespace HexCalc
