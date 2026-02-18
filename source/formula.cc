/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "common.h"

#include "config.h"
#include "formula.h"

using namespace HexCalc;

static bool evaluateError = false;

static inline bool
IsExpression(FormulaTreeNode &node) {
    auto data = node.Get();
    // return true if is number or paired brackets
    if (data.IsNumber() || data.Paired()) {
        return true;
    }

    auto left = node.Left();
    auto right = node.Right();
    if (left == nullptr || right == nullptr) {
        return false;
    }
    // if is an operator,
    // return true if both left and right are valid expressions
    if (IsExpression(*node.Left()) && IsExpression(*node.Left())) {
        return true;
    }

    return false;
}

bool
FormulaTree::Input(const FormulaData &data) {
    if (size >= MaxSize) {
        // tree is full
        return false;
    }

    bool incomingNumber = data.IsNumber();
    bool currentExpression = IsExpression(*currentNode);

    if (!incomingNumber && !currentExpression) {
        // 2 operators cannot be adjacent
        // unless incoming operator is left bracket
        if (data.GetOperator() == LeftBracket) {
            // create a new node for the number
            auto &node = newNode();
            node.Assign(data);

            auto op = currentNode->Get().GetOperator();
            if (Operator::Unary(op)) {
                FormulaTreeNode::ConnectLeft(*currentNode, node);
            } else {
                FormulaTreeNode::ConnectRight(*currentNode, node);
            }
            currentNode = &node;
            return true;
        } else {
            return false;
        }
    } else if (incomingNumber && currentExpression) {
        // data is a number, current node is an expression
        auto currentData = currentNode->Get();
        if (currentData.IsNumber()) {
            auto newValue = data.GetNumber();
            currentNode->Assign(FormulaData(newValue));
        }
    } else if (incomingNumber && !currentExpression) {
        // data is a number, current node is an incomplete operator

        // create a new node for the number
        auto &node = newNode();
        node.Assign(data);

        auto op = currentNode->Get().GetOperator();
        if (Operator::Unary(op)) {
            FormulaTreeNode::ConnectLeft(*currentNode, node);
        } else {
            FormulaTreeNode::ConnectRight(*currentNode, node);
        }

        // point current node to new number
        currentNode = &node;
    } else {
        // !incomingNumber && currentExpression
        // data is an operator and current node is an expression

        // bracket handling
        auto op = data.GetOperator();
        if (op == OperatorType::RightBracket) {
            auto *leftBracket = findNearestUnPairedLeftBracket();
            if (leftBracket == nullptr) {
                return false;
            }
            auto data = leftBracket->Get();
            data.SetPaired(true);
            leftBracket->Assign(data);
            // point current node to the left bracket
            currentNode = leftBracket;

            return true;
        }

        // create a new node for the operator
        auto &node = newNode();
        node.Assign(data);

        // current node must have a parent operator
        auto &parent = *currentNode->Parent();
        auto parentOp = parent.Get().GetOperator();

        if ((parentOp == Equal) || (parentOp == LeftBracket)) {

            if (parent.Left() == currentNode) {
                FormulaTreeNode::ConnectLeft(parent, node);
            } else {
                FormulaTreeNode::ConnectRight(parent, node);
            }
            FormulaTreeNode::ConnectLeft(node, *currentNode);
        } else if (Operator::LowerThan(op, parent.Get().GetOperator())) {
            // new operator has lower precedence than parent operator
            auto grandParent = parent.Parent();
            // new operator becomes the parent of the parent node
            if (grandParent->Left() == &parent) {
                FormulaTreeNode::ConnectLeft(*grandParent, node);
            } else {
                FormulaTreeNode::ConnectRight(*grandParent, node);
            }
            FormulaTreeNode::ConnectLeft(node, parent);
        } else {
            // new operator has higher or equal precedence than parent
            // operator new operator becomes the child of current node
            if (parent.Left() == currentNode) {
                FormulaTreeNode::ConnectLeft(parent, node);
            } else {
                FormulaTreeNode::ConnectRight(parent, node);
            }
            FormulaTreeNode::ConnectLeft(node, *currentNode);
        }
        // point current node to new operator
        currentNode = &node;
    }

    return true;
}

void
FormulaTree::Clear(void) {
    // reset counter
    size = 0;

    // create a new node with number 0
    auto &node = newNode();
    node.Assign(FormulaData(NumberZero, true));
    // connect the new node to the dummy root node
    FormulaTreeNode::ConnectLeft(root, node);
    // point current node to the new node
    currentNode = &node;
}

static void
evaluateNode(FormulaTreeNode &node) {
    if (evaluateError) {
        return;
    }

    auto data = node.Get();
    if (data.IsNumber()) {
        // do nothing if is number
    } else {
        // is operator
        auto op = data.GetOperator();
        auto *left = node.Left();
        auto *right = node.Right();

        if (Operator::Unary(op)) {
            // check left node if is unary operator
            if (left == nullptr) {
                evaluateError = true;
                return;
            }
            // this only handles equal & bracket
            data.SetNumber(left->Get().GetNumber());
            node.Assign(data);
        } else {
            // check both left & right nodes if is binary operator
            if ((left == nullptr) || (right == nullptr)) {
                evaluateError = true;
                return;
            }

            auto lvalue = left->Get().GetNumber();
            auto rvalue = right->Get().GetNumber();

            switch (op) {
            case LeftShift:
                data.SetNumber(Operator::LeftShift(lvalue, rvalue));
                break;
            case RightShift:
                data.SetNumber(Operator::RightShift(lvalue, rvalue));
                break;
            case Modulo:
                data.SetNumber(Operator::Modulo(lvalue, rvalue));
                break;
            case BitwiseAnd:
                data.SetNumber(Operator::And(lvalue, rvalue));
                break;
            case BitwiseOr:
                data.SetNumber(Operator::Or(lvalue, rvalue));
                break;
            case Multiply:
                data.SetNumber(Operator::Multiply(lvalue, rvalue));
                break;
            case Divide:
                data.SetNumber(Operator::Divide(lvalue, rvalue));
                break;
            case Plus:
                data.SetNumber(Operator::Plus(lvalue, rvalue));
                break;
            case Minus:
                data.SetNumber(Operator::Minus(lvalue, rvalue));
                break;
            default:
                // TODO generate error event
                break;
            }

            node.Assign(data);
        }
    }
}

bool
FormulaTree::Evaluate(void) {
    evaluateError = false;

    PostOrderTraversal<FormulaTreeNode, 32>(&root, evaluateNode);

    // auto result = root.Get().GetNumber();

    return evaluateError != true;
}

FormulaTreeNode *
FormulaTree::findNearestUnPairedLeftBracket() {
    for (auto node = currentNode; node != nullptr; node = node->Parent()) {
        auto &nodeRef = *node;

        if (!nodeRef.Get().IsOperator() ||
            nodeRef.Get().GetOperator() != OperatorType::LeftBracket) {
            if (treeNodeIsCompleted(nodeRef)) {
                continue;
            } else {
                return nullptr;
            }
        }
        // Found the matching left bracket
        if (!nodeRef.Get().Paired()) {
            return node;
        }
    }

    return nullptr;
}
