#ifndef CALCULATOR_HPP_
#define CALCULATOR_HPP_

#include "tree.hpp"
#include "expression_elem.hpp"

static const double epsilone = 1e-12;

static const char* TEX_DUMP_BUILD_FILE_NAME = "texdump.tex";
static const char* TEX_DUMP_FILE_NAME = "texdump.pdf";

static const char* FRASES[] = {"очевидно, что", "получим", "таким образом"};

double GetVar(size_t i);

double SetVar(size_t i, double value);

bool IsEqual(double a, double b);

double CalculateTree(Tree* tree);

void DiffTree(Tree* tree, Tree* diffed_tree);

void TexDumpStart();

void CalculatorTreeTexDump(Tree* tree);

void TexDumpEnd();

void TreeConstConv(Tree* tree);

void TreeSimpleOperations(Tree* tree);

void TreeSimplify(Tree* tree);

TreeNode* TreeNodeWithDautherInit(tree_elem_t value, TreeNode* left, TreeNode* right);

#define cL CopySubTree(TreeNodeGetLeft(node))
#define cR CopySubTree(TreeNodeGetRight(node))
#define dL DiffSubTree(TreeNodeGetLeft(node))
#define dR DiffSubTree(TreeNodeGetRight(node))

#define CONST(c) TreeNodeInit({.type = NUM, .value = {.num = c}})

#define ADD(left, right) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = ADD}}, left, right)
#define SUB(left, right) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = SUB}}, left, right)
#define MUL(left, right) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = MUL}}, left, right)
#define DIV(left, right) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = DIV}}, left, right)
#define POW(left, right) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = POW}}, left, right)

#define SIN(P) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = SIN}}, P, CONST(0))
#define ARCSIN(P) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = ARCSIN}}, P, CONST(0))
#define COS(P) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = COS}}, P, CONST(0))
#define ARCCOS(P) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = ARCCOS}}, P, CONST(0))
#define LN(P) TreeNodeWithDautherInit({.type = OPERATION, .value = {.operation = LN}}, P, CONST(0))

#endif // CALCULATOR_HPP_