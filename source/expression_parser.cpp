#include "expression_parser.hpp"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

TreeNode* GetG(char** s) {
    char* loc_s = *s;

    TreeNode* val = GetE(&loc_s);
    if (val == NULL) {
        *s = loc_s;

        return NULL;
    }

    if(*loc_s == '\0') {
        *s = loc_s;

        return val;
    }

    loc_s++;

    *s = loc_s;

    return NULL;
}

TreeNode* GetE(char** s) {
    char* loc_s = *s;

    TreeNode* val = GetT(&loc_s);
    if (val == NULL) {
        *s = loc_s;
        return NULL;
    }

    while (*loc_s == '+' or *loc_s == '-') {
        if (*loc_s == '+') {
            loc_s++;

            TreeNode* add_val = GetT(&loc_s);
            if (add_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* add_node = TreeNodeInit({.type = OPERATION, {.operation = ADD}});
            TreeNodeLinkLeft(add_node, val);
            TreeNodeLinkRight(add_node, add_val);

            val = add_node;
        }
        else {
            loc_s++;

            TreeNode* sub_val = GetT(&loc_s);
            if (sub_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* sub_node = TreeNodeInit({.type = OPERATION, {.operation = SUB}});
            TreeNodeLinkLeft(sub_node, val);
            TreeNodeLinkRight(sub_node, sub_val);

            val = sub_node;
        }
    }

    *s = loc_s;

    return val;
}

TreeNode* GetT(char** s) {
    char* loc_s = *s;

    TreeNode* val = GetPow(&loc_s);
    if (val == NULL) {
        *s = loc_s;

        return NULL;
    }

    while (*loc_s == '*' or *loc_s == '/') {
        if (*loc_s == '*') {
            loc_s++;

            TreeNode* mul_val = GetPow(&loc_s);
            if (mul_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* mul_node = TreeNodeInit({.type = OPERATION, {.operation = MUL}});
            TreeNodeLinkLeft(mul_node, val);
            TreeNodeLinkRight(mul_node, mul_val);

            val = mul_node;
        }
        else {
            loc_s++;

            TreeNode* div_val = GetPow(&loc_s);
            if (div_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* div_node = TreeNodeInit({.type = OPERATION, {.operation = DIV}});
            TreeNodeLinkLeft(div_node, val);
            TreeNodeLinkRight(div_node, div_val);

            val = div_node;
        }
    }

    *s = loc_s;

    return val;
}

TreeNode* GetPow(char** s) {
    char* loc_s = *s;

    TreeNode* val = GetP(&loc_s);
    if (val == NULL) {
        *s = loc_s;

        return NULL;
    }

    if (*loc_s == '^') {
        loc_s++;

        TreeNode* pow_val = GetP(&loc_s);
        if (pow_val == NULL) {
            *s = loc_s;
            return NULL;
        }

        TreeNode* pow_node = TreeNodeInit({.type = OPERATION, {.operation = POW}});
        TreeNodeLinkLeft(pow_node, val);
        TreeNodeLinkRight(pow_node, pow_val);

        val = pow_node;
    }

    *s = loc_s;

    return val;
}

TreeNode* GetP(char** s) {
    char* loc_s = *s;

    TreeNode* val = 0;

    if (*loc_s == '(') {
        loc_s++;
        val = GetE(&loc_s);

        if (*(loc_s++) != ')') {
            *s = loc_s;

            return NULL;
        }
    }
    else if (isdigit(*loc_s)) {
        val = GetN(&loc_s);
    }
    else if (isalpha(*loc_s)) {
        val = GetV(&loc_s);
    }

    *s = loc_s;

    return val;
}

TreeNode* GetN(char** s) {
    char* loc_s = *s;

    int num_val = 0;

    while (isdigit(*loc_s)) {
        num_val *= 10;
        num_val += *loc_s - '0';
        loc_s++;
    }

    TreeNode* val = TreeNodeInit({.type = NUM, {.num = (double)num_val}});

    *s = loc_s;

    return val;
}

TreeNode* GetV(char** s) {
    char* loc_s = *s;

    char var_name[MAX_VAR_NAME] = "";

    int symbol_i = 0;
    while (isalnum(*loc_s)) {
        var_name[symbol_i] = *loc_s;
        loc_s++;
        symbol_i++;
    }

    TreeNode* val = NULL;

    if (strcmp(var_name, "X") == 0) {
        val = TreeNodeInit({.type = VAR, {.num = X}});
    }
    else if (strcmp(var_name, "Y") == 0) {
        val = TreeNodeInit({.type = VAR, {.num = Y}});
    }
    else {
        return NULL;
    }

    *s = loc_s;

    return val;
}

TreeNode* GetF(char** s) {
    char* loc_s = *s;

    char var_name[MAX_VAR_NAME] = "";

    int symbol_i = 0;
    while (isalnum(*loc_s)) {
        var_name[symbol_i] = *loc_s;
        loc_s++;
        symbol_i++;
    }

    TreeNode* val = NULL;

    if (strcmp(var_name, "sin") == 0) { // FIXME sin -> functions[0]

    }
    else if (strcmp(var_name, "arcsin") == 0) {

    }
    else if (strcmp(var_name, "cos") == 0) {

    }
    else if (strcmp(var_name, "arccos") == 0) {

    }
    else if (strcmp(var_name, "ln") == 0) {

    }
    else {
        return NULL;
    }

    *s = loc_s;

    return val;
}