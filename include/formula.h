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

/**
 * @brief A binary tree node that represents either a number or an operator in
 * the formula tree. The tree structure encodes the precedence and associativity
 * of the operators, as well as the grouping of expressions with brackets.
 *
 */
class FormulaTreeNode : public TreeNode<FormulaTreeNode, FormulaData> {
  public:
    FormulaTreeNode(void) : TreeNode() {}
    FormulaTreeNode(const FormulaData &data) : TreeNode(data) {}

    /**
     * @brief Evaluate the expression represented by this node. If the node is a
     * number, do nothing. If the node is an operator, evaluate its children and
     * apply the operator to get the result.
     *
     */
    void Evaluate(void);

    /**
     * @brief Check if the node is a valid expression. A valid expression can be
     * a number, a paired bracket, or an operator with valid expressions as its
     * children.
     *
     * @return true if the node is a valid expression, false otherwise
     */
    bool Expression();

    /**
     * @brief Check if the node is completed, which means it has enough children
     * to be evaluated. For number node, it is always completed. For operator
     * node, it is completed if it has at least 1 child for unary operator, or
     * at least 2 children for binary operator.
     *
     * @return true if the node is completed, false otherwise
     */
    bool Completed();

    /**
     * @brief Find the nearest unpaired left bracket node from the current node.
     *
     * @return FormulaTreeNode* pointer to the unpaired '(' node, or nullptr if
     * not found
     */
    FormulaTreeNode *findUnpairedLBrac();
};

class FormulaTree : private NonCopyable {
  public:
    explicit FormulaTree(void);

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
     * @brief The array of pre-allocated tree nodes.
     */
    FormulaTreeNode nodes[MaxSize];
    /**
     * @brief root node, will be enclosed by another equal operator node.
     */
    FormulaTreeNode root;
    FormulaTreeNode *currentNode;
    size_t size;

    /**
     * @brief Create a new node from the pre-allocated array and initialize it
     * with default value.
     *
     * @return FormulaTreeNode& reference to the new node
     */
    FormulaTreeNode &newNode();
};

}; // namespace HexCalc
