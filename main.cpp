#include <stdio.h>

#include "tree.hpp"
#include "calculator.hpp"

int main() {
    Tree calculator_tree = {};
    TreeInit(&calculator_tree);

    TreeNode* div = TreeNodeInit({.type = OPERATION, .value = {.operation = DIV}});
    TreeNodeLinkLeft(calculator_tree.root, div);

    TreeNode* dividend = TreeNodeInit({.type = OPERATION, .value = {.operation = ADD}});
    TreeNodeLinkLeft(div, dividend);

    TreeNode* dividend_left = TreeNodeInit({.type = NUM, .value = {.num = 1}});
    TreeNodeLinkLeft(dividend, dividend_left);

    TreeNode* dividend_right = TreeNodeInit({.type = VAR, .value = {.var = X}});
    TreeNodeLinkRight(dividend, dividend_right);

    TreeNode* divisor = TreeNodeInit({.type = OPERATION, .value = {.operation = SUB}});
    TreeNodeLinkRight(div, divisor);

    TreeNode* divisor_left = TreeNodeInit({.type = NUM, .value = {.num = 1}});
    TreeNodeLinkLeft(divisor, divisor_left);

    TreeNode* divisor_right = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
    TreeNodeLinkRight(divisor, divisor_right);

    TreeNode* divisor_right_left = TreeNodeInit({.type = VAR, .value = {.var = X}});
    TreeNodeLinkLeft(divisor_right, divisor_right_left);

    TreeNode* divisor_right_right = TreeNodeInit({.type = VAR, .value = {.var = X}});
    TreeNodeLinkRight(divisor_right, divisor_right_right);

    TREE_DUMP(&calculator_tree);

    Tree diffed_tree = {};
    TreeInit(&diffed_tree);

    DiffTree(&calculator_tree, &diffed_tree);

    TREE_DUMP(&diffed_tree);

    TreeConstConv(&diffed_tree);
    
    TreeSimpleOperations(&diffed_tree);

    TREE_DUMP(&diffed_tree);

    CalculatorTreeTexDump(&diffed_tree);

    TreeDestroy(&diffed_tree);

    TreeDestroy(&calculator_tree);
    
    return 0;
}