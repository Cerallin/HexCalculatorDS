/**
 * HexCalculatorDS - An integer calculator for Nintendo DS.
 * Copyright (C) 2026  Cerallin <cerallin@cerallin.top>
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */
#include "test_config.h"

#include "config.cc"
#include "formula.cc"
#include "operator.cc"

static void
configNumber(NumberBase b, NumberWidth w, NumberSign s) {
    Event event;

    event = {
        .data = b,
        .type = UpdateBaseEvent,
    };
    config.HandleEvent(event);

    event = {
        .data = w,
        .type = UpdateWidthEvent,
    };
    config.HandleEvent(event);

    event = {
        .data = s,
        .type = UpdateSignEvent,
    };
    config.HandleEvent(event);
}

HexCalc::FormulaTree *formula;

TEST_GROUP(FormulaTree){void setup(){configNumber(Decimal, QWord, Unsigned);
formula = new HexCalc::FormulaTree;
}
void
teardown() {
    delete formula;
}
}
;

TEST(FormulaTree, TestInputSingleNumber) {
    HexCalc::FormulaData num(42);
    CHECK(formula->Input(num));
}

TEST(FormulaTree, TestInputSingleOperator) {
    HexCalc::FormulaData op(HexCalc::OperatorType::Add);
    CHECK(formula->Input(op));
}

TEST(FormulaTree, TestEvaluateEmpty) { CHECK(formula->Evaluate()); }

TEST(FormulaTree, TestSimpleAdd) {
    // 1 + 2 = 3
    HexCalc::FormulaData n1(1);
    HexCalc::FormulaData n2(2);
    HexCalc::FormulaData op(HexCalc::OperatorType::Add);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(3, formula->Result());
}

TEST(FormulaTree, TestSimpleMultiply) {
    // 2 x 5 = 10
    HexCalc::FormulaData n1(2);
    HexCalc::FormulaData n2(5);
    HexCalc::FormulaData op(HexCalc::OperatorType::Multiply);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(10, formula->Result());
}

TEST(FormulaTree, TestSimpleLeftShift) {
    // 2 << 5 = 64
    HexCalc::FormulaData n1(2);
    HexCalc::FormulaData n2(5);
    HexCalc::FormulaData op(HexCalc::OperatorType::LeftShift);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(64, formula->Result());
}

TEST(FormulaTree, TestSimpleRightShiftToZero) {
    // 2 >> 5 = 0
    HexCalc::FormulaData n1(2);
    HexCalc::FormulaData n2(5);
    HexCalc::FormulaData op(HexCalc::OperatorType::RightShift);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(0, formula->Result());
}

TEST(FormulaTree, TestByteOverflow) {
    // 0xFD + 0xAC = 0xA9
    configNumber(Decimal, Byte, Unsigned);

    HexCalc::FormulaData n1(0xFD);
    HexCalc::FormulaData n2(0xAC);
    HexCalc::FormulaData op(HexCalc::OperatorType::Add);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(0xA9, formula->Result());
}

TEST(FormulaTree, TestSimpleModulo) {
    // 114514 % 7 = 1
    HexCalc::FormulaData n1(114514);
    HexCalc::FormulaData n2(7);
    HexCalc::FormulaData op(HexCalc::OperatorType::Modulo);

    CHECK(formula->Input(n1));
    CHECK(formula->Input(op));
    CHECK(formula->Input(n2));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(1, formula->Result());
}

TEST(FormulaTree, TestOperatorPrecedence) {
    // 1 + 2 x 6 = 13
    using HexCalc::OperatorType;

    CHECK(formula->Input(HexCalc::FormulaData(1)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(6)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(13, formula->Result());
}

TEST(FormulaTree, TestOperatorPrecedence2) {
    // 2 x 3 + 99 = 105
    using HexCalc::OperatorType;

    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(3)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(99)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(105, formula->Result());
}

TEST(FormulaTree, TestSimpleBrackets) {
    // (1 + 2) x 6 = 18
    using HexCalc::OperatorType;

    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(1)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(6)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(18, formula->Result());
}

TEST(FormulaTree, TestMoreBrackets) {
    // (1 + 2) x (5 + 6) = 33
    using HexCalc::OperatorType;

    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(1)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(5)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(6)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(33, formula->Result());
}

TEST(FormulaTree, TestNestedBrackets) {
    // 1 + ((2) + 3) + ((4)) = 10
    using HexCalc::OperatorType;

    CHECK(formula->Input(HexCalc::FormulaData(1)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(3)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Add)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(4)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(10, formula->Result());
}

TEST(FormulaTree, TestComplexFormula) {
    // ((4 - 1) x 2) Lsh ((75 AND 15 x 3) / 4) = -768
    using HexCalc::OperatorType;

    configNumber(Decimal, QWord, Signed);

    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(4)));
    CHECK(formula->Input(HexCalc::FormulaData(Minus)));
    CHECK(formula->Input(HexCalc::FormulaData(1)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(2)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftShift)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(LeftBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(75)));
    CHECK(formula->Input(HexCalc::FormulaData(BitwiseAnd)));
    CHECK(formula->Input(HexCalc::FormulaData(15)));
    CHECK(formula->Input(HexCalc::FormulaData(Multiply)));
    CHECK(formula->Input(HexCalc::FormulaData(3)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));
    CHECK(formula->Input(HexCalc::FormulaData(Divide)));
    CHECK(formula->Input(HexCalc::FormulaData(4)));
    CHECK(formula->Input(HexCalc::FormulaData(RightBracket)));

    CHECK(formula->Evaluate());
    CHECK_EQUAL(24, formula->Result());
}

int
main(int ac, char **av) {
    return CommandLineTestRunner::RunAllTests(ac, av);
}
