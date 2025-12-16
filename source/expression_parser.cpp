#include "expression_parser.hpp"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "calculator.hpp"

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

            TreeNode* add_node = ADD(val, add_val);

            val = add_node;
        }
        else {
            loc_s++;

            TreeNode* sub_val = GetT(&loc_s);
            if (sub_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* sub_node = SUB(val, sub_val);

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

            TreeNode* mul_node = MUL(val, mul_val);

            val = mul_node;
        }
        else {
            loc_s++;

            TreeNode* div_val = GetPow(&loc_s);
            if (div_val == NULL) {
                *s = loc_s;
                return NULL;
            }

            TreeNode* div_node = DIV(val, div_val);

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

        TreeNode* pow_node = POW(val, pow_val);

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
        char name[MAX_NAME_LEN] = "";

        int symbol_i = 0;
        while (isalnum(*loc_s)) {
            if (symbol_i >= MAX_NAME_LEN - 1) {
                return NULL;
            }

            name[symbol_i] = *loc_s;
            loc_s++;
            symbol_i++;
        }

        loc_s -= symbol_i;

        bool is_func = false;
        int func_cnt = sizeof(OPERATION_NAME) / sizeof(OPERATION_NAME[0]);
        for (int func_i = 0; func_i < func_cnt; func_i++) {
            if (strcmp(name, OPERATION_NAME[func_i]) == 0) {
                is_func = true;
            }
        }
        if (is_func) {
            val = GetF(&loc_s);
        }
        else {
            val = GetV(&loc_s);
        }
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

    char var_name[MAX_NAME_LEN] = "";

    int symbol_i = 0;
    while (isalnum(*loc_s)) {
        if (symbol_i >= MAX_NAME_LEN - 1) {
            return NULL;
        }

        var_name[symbol_i] = *loc_s;
        loc_s++;
        symbol_i++;
    }

    TreeNode* val = NULL;

    if (strcmp(var_name, "X") == 0 || strcmp(var_name, "x") == 0) {
        val = TreeNodeInit({.type = VAR, {.num = X}});
    }
    else if (strcmp(var_name, "Y") == 0 || strcmp(var_name, "y") == 0) {
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

    char func_name[MAX_NAME_LEN] = "";

    int symbol_i = 0;
    while (isalnum(*loc_s)) {
        if (symbol_i >= MAX_NAME_LEN - 1) {
            return NULL;
        }

        func_name[symbol_i] = *loc_s;
        loc_s++;
        symbol_i++;
    }

    if (*loc_s == '(') {
        loc_s++;
    }
    else {
        return NULL;
    }

    TreeNode* val = NULL;

    if (strcmp(func_name, "sin") == 0) { // FIXME sin -> functions[0]?
        val = SIN(GetE(&loc_s));
    }
    else if (strcmp(func_name, "arcsin") == 0) {
        val = ARCSIN(GetE(&loc_s));
    }
    else if (strcmp(func_name, "cos") == 0) {
        val = COS(GetE(&loc_s));
    }
    else if (strcmp(func_name, "arccos") == 0) {
        val = ARCCOS(GetE(&loc_s));
    }
    else if (strcmp(func_name, "ln") == 0) {
        val = LN(GetE(&loc_s));
    }
    else {
        return NULL;
    }

    if (*loc_s == ')') {
        loc_s++;
    }
    else {
        return NULL;
    }

    *s = loc_s;

    return val;
}