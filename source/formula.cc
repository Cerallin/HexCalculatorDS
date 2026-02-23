/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "formula.h"
#include "common.h"

using namespace HexCalc;

FormulaTreeNode::EvaluateFlag
FormulaTreeNode::Evaluate(void) {
    EvaluateFlag flag;

    if (value.IsNumber()) {
        // do nothing if is number
    } else {
        // is operator
        auto op = value.GetOperator();

        if (Operator::Unary(op)) {
            // check left node if is unary operator
            if (left == nullptr) {
                flag.invalidExpressionFlag = true;
                return flag;
            }
            // this only handles equal & bracket
            value.SetNumber(left->Get().GetNumber());
            Assign(value);
        } else {
            // check both left & right nodes if is binary operator
            if ((left == nullptr) || (right == nullptr)) {
                flag.invalidExpressionFlag = true;
                return flag;
            }

            auto lvalue = left->Get().GetNumber();
            auto rvalue = right->Get().GetNumber();

            switch (op) {
            case LeftShift:
                value.SetNumber(Operator::LeftShift(lvalue, rvalue));
                break;
            case RightShift:
                value.SetNumber(Operator::RightShift(lvalue, rvalue));
                break;
            case Modulo:
                value.SetNumber(Operator::Modulo(lvalue, rvalue));
                break;
            case BitwiseAnd:
                value.SetNumber(Operator::And(lvalue, rvalue));
                break;
            case BitwiseOr:
                value.SetNumber(Operator::Or(lvalue, rvalue));
                break;
            case Multiply:
                value.SetNumber(Operator::Multiply(lvalue, rvalue));
                break;
            case Divide:
                if (rvalue == 0) {
                    // Divide by zero
                    flag.divideByZeroFlag = true;
                    return flag;
                }
                value.SetNumber(Operator::Divide(lvalue, rvalue));
                break;
            case Plus:
                value.SetNumber(Operator::Plus(lvalue, rvalue));
                break;
            case Minus:
                value.SetNumber(Operator::Minus(lvalue, rvalue));
                break;
            default:
                // TODO generate error event
                flag.unknownOperatorFlag = true;
                return flag;
            }

            Assign(value);
        }
    }

    return flag;
}

bool
FormulaTreeNode::Expression() {
    // return true if is number or paired brackets
    if (value.IsNumber() || value.Paired()) {
        return true;
    }

    auto left = Left();
    auto right = Right();
    if (left == nullptr || right == nullptr) {
        return false;
    }
    // if is an operator,
    // return true if both left and right are valid expressions
    // TODO post-order traversal
    if (left->Expression() && right->Expression()) {
        return true;
    }

    return false;
}

bool
FormulaTreeNode::Completed() {
    if (value.IsNumber()) {
        return true;
    }
    if (value.IsOperator()) {
        auto op = value.GetOperator();
        if (Operator::Unary(op)) {
            return ChildCount() >= 1;
        } else {
            return ChildCount() >= 2;
        }
    }
    return false;
}

FormulaTreeNode *
FormulaTreeNode::findUnpairedLBrac() {
    for (auto node = this; node != nullptr; node = node->Parent()) {
        auto &nodeRef = *node;

        if (!nodeRef.Get().IsOperator() ||
            nodeRef.Get().GetOperator() != OperatorType::LeftBracket) {
            if (nodeRef.Completed()) {
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

FormulaTree::FormulaTree(void)
    : nodes{}, root{FormulaData{OperatorType::Equal}}, currentNode(&nodes[0]),
      size(0) {
    Clear();
}

bool
FormulaTree::Input(const FormulaData &data) {
    bool res = inputData(data);
    if (res) {
        // reset evaluation flag when new data is input
        evaluated = false;
    }
    return res;
}

bool
FormulaTree::inputData(const FormulaData &data) {
    if (size >= MaxSize) {
        // tree is full
        return false;
    }

    bool incomingNumber = data.IsNumber();
    bool currentExpression = currentNode->Expression();

    if (!incomingNumber && !currentExpression) {
        // 2 operators cannot be adjacent
        // unless incoming operator is left bracket
        if (data.GetOperator() == LeftBracket) {
            // create a new node for the number
            auto &node = newNode();
            node.Assign(data);

            auto op = currentNode->Get().GetOperator();
            if (Operator::Unary(op)) {
                currentNode->ConnectLeft(node);
            } else {
                currentNode->ConnectRight(node);
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
            currentNode->ConnectLeft(node);
        } else {
            currentNode->ConnectRight(node);
        }

        // point current node to new number
        currentNode = &node;
    } else {
        // !incomingNumber && currentExpression
        // data is an operator and current node is an expression

        // bracket handling
        auto op = data.GetOperator();
        if (op == OperatorType::RightBracket) {
            auto *leftBracket = currentNode->findUnpairedLBrac();
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
                parent.ConnectLeft(node);
            } else {
                parent.ConnectRight(node);
            }
            node.ConnectLeft(*currentNode);
        } else if (Operator::LowerThan(op, parent.Get().GetOperator())) {
            // new operator has lower precedence than parent operator
            auto grandParent = parent.Parent();
            // new operator becomes the parent of the parent node
            if (grandParent->Left() == &parent) {
                grandParent->ConnectLeft(node);
            } else {
                grandParent->ConnectRight(node);
            }
            node.ConnectLeft(parent);
        } else {
            // new operator has higher or equal precedence than parent
            // operator new operator becomes the child of current node
            if (parent.Left() == currentNode) {
                parent.ConnectLeft(node);
            } else {
                parent.ConnectRight(node);
            }
            node.ConnectLeft(*currentNode);
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
    root.ConnectLeft(node);
    // point current node to the new node
    currentNode = &node;
}

bool
FormulaTree::Evaluate(void) {
    // maximum depth of the tree
    constexpr size_t stackSize = MaxSize / 2;

    bool evaluateError = false;

    root.PostOrderTraversal<stackSize>([&evaluateError](FormulaTreeNode &node) {
        if (evaluateError) {
            return;
        }

        auto flags = node.Evaluate();
        if (!flags.AllClear()) {
            evaluateError = true;
        }
    });

    if (!evaluateError) {
        evaluated = true;
        fullEvaluationFlag = true;
    }

    return (evaluateError != true);
}

bool
FormulaTree::EvaluatePartial(void) {
    constexpr size_t stackSize = MaxSize / 2; // maximum depth of the tree

    bool evaluateError = false;

    auto *node = currentNode->findUnpairedLBrac();
    if (node == nullptr) {
        return false;
    }

    node->PostOrderTraversal<stackSize>(
        [&evaluateError](FormulaTreeNode &node) {
            if (evaluateError) {
                return;
            }

            auto flags = node.Evaluate();
            if (!flags.AllClear()) {
                evaluateError = true;
            }
        });

    if (!evaluateError) {
        evaluated = true;
        fullEvaluationFlag = false;
    }

    return (evaluateError != true);
}

NumberDataType
FormulaTree::Result(void) const {
    if (fullEvaluationFlag) {
        return root.Get().GetNumber();
    } else {
        auto *partialRoot = currentNode->findUnpairedLBrac();
        return partialRoot->Get().GetNumber();
    }
}

FormulaTreeNode &
FormulaTree::newNode() {
    auto node = &nodes[size++];
    node->Reset();

    return *node;
}
