#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "tree.hpp"
#include "calculator.hpp"
#include "expression_parser.hpp"

int main() {
    TexDumpStart();

    Tree calculator_tree = {};
    TreeInit(&calculator_tree);

    char* s = NULL;
    size_t sn = 0;

    getline(&s, &sn, stdin);
    char* start_s = s;
    s[strlen(s) - 1] = '\0';

    TreeNodeLinkLeft(calculator_tree.root, GetG(&s));
    
    if (calculator_tree.root->left == NULL) {
        fprintf(stderr, "Syntax Error!\n");
        return 1;
    }
    
    CalculatorTreeTexDump(&calculator_tree);
    
    TreeSimplify(&calculator_tree);
    
    printf("Производную какого порядка вы хотите взять?:\n");
    size_t n = 0;
    scanf("%lu", &n);

    if (n == 0) {
        printf("Не имеет смысла\n");
        return 0;
    }

    Tree diff_tree = {};
    TreeInit(&diff_tree);
    
    DiffTree(&calculator_tree, &diff_tree);

    for (size_t diff_i = 0; diff_i < n - 1; diff_i++) {
        DiffTree(&diff_tree, &diff_tree);
        TreeSimplify(&diff_tree);
    }
    
    CalculatorTreeTexDump(&diff_tree);
    
    TreeSimplify(&diff_tree);
    
    TREE_DUMP(&diff_tree);
    
    free(start_s);
    
    TreeDestroy(&diff_tree);
    
    TreeDestroy(&calculator_tree);
    
    TexDumpEnd();
    
    return 0;
}