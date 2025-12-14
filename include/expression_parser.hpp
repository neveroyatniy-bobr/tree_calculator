#ifndef EXPRESSION_PARSER_HPP_
#define EXPRESSION_PARSER_HPP_

#include "tree.hpp"

#define MAX_VAR_NAME 128

const char* functions[] = {"sin", "arcsin", "cos", "arccos", "ln"};

TreeNode* GetG(char** s);

TreeNode* GetE(char** s);

TreeNode* GetT(char** s);

TreeNode* GetP(char** s);

TreeNode* GetN(char** s);

TreeNode* GetV(char** s);

TreeNode* GetF(char** s);

#endif // EXPRESSION_PARSER_HPP_