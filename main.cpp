#include <stdio.h>
#include <string.h>

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

    if (calculator_tree.root == NULL) {
        fprintf(stderr, "Syntax Error!\n");
        return 1;
    }

    double ans = CalculateTree(&calculator_tree);

    printf("The answer is: %lf\n", ans);

    free(start_s);

    TreeDestroy(&calculator_tree);
    
    return 0;
}