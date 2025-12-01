#ifndef EXPRESSION_ELEM_HPP_
#define EXPRESSION_ELEM_HPP_

enum Operation {
    ADD = 0,
    SUB = 1,
    MUL = 2,
    DIV = 3
};

enum Var {
    X = 0,
    Y = 1
};

union ExpressionElemValue {
    double num;
    Operation operation;
    Var var;
};

enum ExpressionElemType {
    NUM       = 0,
    OPERATION = 1,
    VAR       = 2
};

struct ExpressionElem {
    ExpressionElemType type;
    ExpressionElemValue value;
};

#endif // EXPRESSION_ELEM_HPP_