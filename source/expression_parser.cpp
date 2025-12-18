#include "expression_parser.hpp"

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "calculator.hpp"

TreeNode* GetG(char** s) {
    assert(s != NULL);

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
    assert(s != NULL);

    char* loc_s = *s;

    TreeNode* val = GetT(&loc_s);
    if (val == NULL) {
        *s = loc_s;
        return NULL;
    }

    while (*loc_s == '+' or *loc_s == '-') {
        loc_s++;

        TreeNode* op_val = GetT(&loc_s);
        if (op_val == NULL) {
            *s = loc_s;
            return NULL;
        }

        TreeNode* op_node = NULL; 

        if (*loc_s == '+') {
            op_node = ADD(val, op_val);
        }
        else {
            op_node = SUB(val, op_val);
        }

        val = op_node;
    }

    *s = loc_s;

    return val;
}

TreeNode* GetT(char** s) {
    assert(s != NULL);

    char* loc_s = *s;

    TreeNode* val = GetPow(&loc_s);
    if (val == NULL) {
        *s = loc_s;

        return NULL;
    }

    while (*loc_s == '*' or *loc_s == '/') {
        loc_s++;

        TreeNode* op_val = GetPow(&loc_s);
        if (op_val == NULL) {
            *s = loc_s;
            return NULL;
        }

        TreeNode* op_node = NULL;

        if (*loc_s == '*') {
            op_node = MUL(val, op_val);
        }
        else {
            op_node = DIV(val, op_val);
        }

        val = op_node;
    }

    *s = loc_s;

    return val;
}

TreeNode* GetPow(char** s) {
    assert(s != NULL);

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

static bool IsFunc(char** s) {
    assert(s != NULL);

    char* loc_s = *s;

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

    *s = loc_s;

    return is_func;
}

TreeNode* GetP(char** s) {
    assert(s != NULL);

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
        
        if (IsFunc(&loc_s)) {
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
    assert(s != NULL);

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
    assert(s != NULL); 

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
        val = TreeNodeInit({.type = VAR, {.var = X}});
    }
    else if (strcmp(var_name, "Y") == 0 || strcmp(var_name, "y") == 0) {
        val = TreeNodeInit({.type = VAR, {.var= Y}});
    }
    else {
        return NULL;
    }

    *s = loc_s;

    return val;
}

TreeNode* GetF(char** s) {
    assert(s != NULL);

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

    if (strcmp(func_name, OPERATION_NAME[SIN]) == 0) {
        val = SIN(GetE(&loc_s));
    }
    else if (strcmp(func_name, OPERATION_NAME[ARCSIN]) == 0) {
        val = ARCSIN(GetE(&loc_s));
    }
    else if (strcmp(func_name, OPERATION_NAME[COS]) == 0) {
        val = COS(GetE(&loc_s));
    }
    else if (strcmp(func_name, OPERATION_NAME[ARCCOS]) == 0) {
        val = ARCCOS(GetE(&loc_s));
    }
    else if (strcmp(func_name, OPERATION_NAME[LN]) == 0) {
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