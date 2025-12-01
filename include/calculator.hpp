#ifndef CALCULATOR_HPP_
#define CALCULATOR_HPP_

#include "tree.hpp"
#include "expression_elem.hpp"

static const double epsilone = 1e-12;

double GetVar(size_t i);

double SetVar(size_t i, double value);

bool IsEqual(double a, double b);

double CalculateTree(Tree* tree);

void DiffTree(Tree* tree, Tree* diffed_tree);

void CalculatorTreeTexDump(Tree* tree);

void TreeConstConv(Tree* tree);

void TreeSimpleOperations(Tree* tree);

#endif // CALCULATOR_HPP_