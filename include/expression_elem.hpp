#ifndef EXPRESSION_ELEM_HPP_
#define EXPRESSION_ELEM_HPP_

enum Operation {
    ADD       = 0,
    SUB       = 1,
    MUL       = 2,
    DIV       = 3,
    POW       = 4,
    SIN       = 5,
    ARCSIN    = 6,
    COS       = 7,
    ARCCOS    = 8,
    LN        = 9
};

enum Var {
    X = 0,
    Y = 1
};

enum DummyType {
    ROOT             = 0,
    ROOT_RIGHT       = 1,
    EXPRESSION_RIGHT = 2
};

union ExpressionElemValue {
    double num;
    Operation operation;
    Var var;
    DummyType dummy_type;
};

enum ExpressionElemType {
    NUM       = 0,
    OPERATION = 1,
    VAR       = 2,
    DUMMY     = 3
};

struct ExpressionElem {
    ExpressionElemType type;
    ExpressionElemValue value;
};

#endif // EXPRESSION_ELEM_HPP_