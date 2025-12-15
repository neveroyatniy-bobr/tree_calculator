#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tree.hpp"
#include "calculator.hpp"
#include "expression_parser.hpp"

int main() {
    Tree calculator_tree = {};
    TreeInit(&calculator_tree);

    char* s = NULL;
    size_t n = 0;

    getline(&s, &n, stdin);
    char* start_s = s;
    s[strlen(s) - 1] = '\0';

    TreeNodeLinkLeft(calculator_tree.root, GetG(&s));

    TREE_DUMP(&calculator_tree);

    if (calculator_tree.root->left == NULL) {
        fprintf(stderr, "Syntax Error!\n");
        return 1;
    }

    double ans = CalculateTree(&calculator_tree);

    printf("The answer is: %lf\n", ans);

    Tree diff_tree = {};
    TreeInit(&diff_tree);
    
    TREE_DUMP(&calculator_tree);
    
    DiffTree(&calculator_tree, &diff_tree);

    TREE_DUMP(&diff_tree);

    TreeConstConv(&diff_tree);
    TreeSimpleOperations(&diff_tree);
    TreeConstConv(&diff_tree);
    TreeSimpleOperations(&diff_tree);
    TreeConstConv(&diff_tree);
    TreeSimpleOperations(&diff_tree);
    TREE_DUMP(&diff_tree);
    
    CalculatorTreeTexDump(&diff_tree);

    free(start_s);

    TreeDestroy(&diff_tree);

    TreeDestroy(&calculator_tree);
    
    return 0;
}