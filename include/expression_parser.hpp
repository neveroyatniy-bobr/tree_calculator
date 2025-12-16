#ifndef EXPRESSION_PARSER_HPP_
#define EXPRESSION_PARSER_HPP_

#include "tree.hpp"

#define MAX_NAME_LEN 128

TreeNode* GetG(char** s);

TreeNode* GetE(char** s);

TreeNode* GetT(char** s);

TreeNode* GetPow(char** s);

TreeNode* GetP(char** s);

TreeNode* GetN(char** s);

TreeNode* GetV(char** s);

TreeNode* GetF(char** s);

#endif // EXPRESSION_PARSER_HPP_