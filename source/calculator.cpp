#include "calculator.hpp"

#include <stdio.h>
#include <math.h>

#include "expression_elem.hpp"
#include "tree.hpp"

//FIXME хочу чтобы сам читал количество членов enum
static double var[2] = {0, 0};

double GetVar(size_t i) {
    return var[i];
}

double SetVar(size_t i, double value) {
    return var[i] = value;
}

bool IsEqual(double a, double b) {
    return fabs(a - b) < epsilone;
}

static double CalculateSubTree(TreeNode* node) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    if (node_value.type == OPERATION) {
        double left_sub_tree_value = CalculateSubTree(TreeNodeGetLeft(node));
        double right_sub_tree_value = CalculateSubTree(TreeNodeGetRight(node));
        switch (node_value.value.operation) {
            case ADD:
                return left_sub_tree_value + right_sub_tree_value;
            case SUB:
                return left_sub_tree_value - right_sub_tree_value;
            case MUL:
                return left_sub_tree_value * right_sub_tree_value;
            case DIV:
                return left_sub_tree_value / right_sub_tree_value;
            case SIN:
                return sin(left_sub_tree_value);
            case ARCSIN:
                return asin(left_sub_tree_value);
            case COS:
                return cos(left_sub_tree_value);
            case ARCCOS:
                return acos(left_sub_tree_value);
            case LN:
                return log(left_sub_tree_value);
            default:
                return 0;
        }
    }

    if (node_value.type == VAR) {
        return GetVar(node_value.value.var);
    }
    
    return node_value.value.num;
}

double CalculateTree(Tree* tree) {
    return CalculateSubTree(TreeNodeGetLeft(tree->root));
}

static TreeNode* CopySubTree(TreeNode* node) {
    if (node == NULL) {
        return NULL;
    }

    TreeNode* new_left_sub_tree = CopySubTree(TreeNodeGetLeft(node));
    TreeNode* new_right_sub_tree = CopySubTree(TreeNodeGetRight(node));

    TreeNode* new_node = TreeNodeInit(TreeNodeGetValue(node));
    TreeNodeLinkLeft(new_node, new_left_sub_tree);
    TreeNodeLinkRight(new_node, new_right_sub_tree);

    return new_node;
}

static TreeNode* DiffSubTree(TreeNode* node) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    if (node_value.type == OPERATION) {
        switch (node_value.value.operation) {
            case ADD: {
                TreeNode* new_first = TreeNodeInit({.type = OPERATION, .value = {.operation = ADD}});
                TreeNodeLinkLeft(new_first, DiffSubTree(TreeNodeGetLeft(node)));
                TreeNodeLinkRight(new_first, DiffSubTree(TreeNodeGetRight(node)));
                
                return new_first;
            }
            case SUB: {
                TreeNode* new_first = TreeNodeInit({.type = OPERATION, .value = {.operation = SUB}});
                TreeNodeLinkLeft(new_first, DiffSubTree(TreeNodeGetLeft(node)));
                TreeNodeLinkRight(new_first, DiffSubTree(TreeNodeGetRight(node)));

                return new_first;
            }
            case MUL: {
                TreeNode* new_first = TreeNodeInit({.type = OPERATION, .value = {.operation = ADD}});

                TreeNode* left_mul = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
                TreeNodeLinkLeft(new_first, left_mul);

                TreeNode* right_mul = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
                TreeNodeLinkRight(new_first, right_mul);

                TreeNodeLinkLeft(left_mul, DiffSubTree(TreeNodeGetLeft(node)));
                TreeNodeLinkRight(left_mul, CopySubTree(TreeNodeGetRight(node)));

                TreeNodeLinkLeft(right_mul, DiffSubTree(TreeNodeGetRight(node)));
                TreeNodeLinkRight(right_mul, CopySubTree(TreeNodeGetLeft(node)));

                return new_first;
            }
            case DIV: {
                TreeNode* new_first = TreeNodeInit({.type = OPERATION, .value = {.operation = DIV}});

                TreeNode* dividend  = TreeNodeInit({.type = OPERATION, .value = {.operation = SUB}});
                TreeNodeLinkLeft(new_first, dividend);

                TreeNode* left_mul = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
                TreeNodeLinkLeft(dividend, left_mul);

                TreeNode* right_mul = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
                TreeNodeLinkRight(dividend, right_mul);

                TreeNodeLinkLeft(left_mul, DiffSubTree(TreeNodeGetLeft(node)));
                TreeNodeLinkRight(left_mul, CopySubTree(TreeNodeGetRight(node)));

                TreeNodeLinkLeft(right_mul, DiffSubTree(TreeNodeGetRight(node)));
                TreeNodeLinkRight(right_mul, CopySubTree(TreeNodeGetLeft(node)));

                TreeNode* divisor = TreeNodeInit({.type = OPERATION, .value = {.operation = MUL}});
                TreeNodeLinkRight(new_first, divisor);
                
                TreeNodeLinkLeft(divisor, CopySubTree(TreeNodeGetRight(node)));
                TreeNodeLinkRight(divisor, CopySubTree(TreeNodeGetRight(node)));

                return new_first;
            }

            case SIN: {
                TreeNode* new_first = TreeNodeInit({.type = OPERATION, {.operation = MUL}});
                
                TreeNode* left_mul = TreeNodeInit({.type = OPERATION, {.operation = COS}});
                TreeNodeLinkLeft(new_first, left_mul);

                TreeNode* left_cos = CopySubTree(TreeNodeGetLeft(node));
                TreeNodeLinkLeft(left_mul, left_cos);
                TreeNode* right_cos = TreeNodeInit({.type = NUM, {.num = 0}});
                TreeNodeLinkRight(left_mul, right_cos);

                TreeNode* right_mul = DiffSubTree(TreeNodeGetLeft(node));
            }

            case ARCSIN: {

            }

            case COS: {

            }

            case ARCCOS: {

            }

            case LN: {

            }

            default:
                return 0;
        }
    }

    if (node_value.type == VAR) {
        TreeNode* new_first = TreeNodeInit({.type = NUM, .value = {.num = 1}});

        return new_first;
    }

    TreeNode* new_first = TreeNodeInit({.type = NUM, .value = {.num = 0}});

    return new_first;
}

void DiffTree(Tree* tree, Tree* diffed_tree) {
    TreeNodeLinkLeft(TreeGetRoot(diffed_tree), DiffSubTree(TreeNodeGetLeft(TreeGetRoot(tree))));
}

static void CalculatorSubTreeTexDump(TreeNode* node) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    if (node_value.type == OPERATION) {
        printf("(");
        switch (node_value.value.operation) {
            case ADD: {
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf(" + ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node));

                printf(")");

                return;
            }
            case SUB: {
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf(" - ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node));

                printf(")");

                return;
            }
            case MUL: {
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf(" \\cdot ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node));

                printf(")");

                return;
            }
            case DIV: {
                printf("\\frac{");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf("}{");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node));
                printf("}");

                printf(")");
                
                return;
            }
            default:
                return;
        }
    }

    if (node_value.type == VAR) {
        switch (node_value.value.var)
        {
        case X:
            printf("x");
            return;
        case Y:
            printf("y");
            return;
        default:
            return;
        }
    }

    printf("%.3lf", node_value.value.num);
}

void CalculatorTreeTexDump(Tree* tree) {
    return CalculatorSubTreeTexDump(TreeNodeGetLeft(TreeGetRoot(tree)));
}

static void SubTreeConstConv(TreeNode* node) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    if (node_value.type == OPERATION) {
        SubTreeConstConv(TreeNodeGetLeft(node));
        SubTreeConstConv(TreeNodeGetRight(node));

        ExpressionElemType left_sub_tree_type = TreeNodeGetValue(TreeNodeGetLeft(node)).type;
        ExpressionElemType right_sub_tree_type = TreeNodeGetValue(TreeNodeGetRight(node)).type;

        if (left_sub_tree_type == NUM && right_sub_tree_type == NUM) {
            double new_node_value = CalculateSubTree(node);
            TreeNodeSetValue(node, {.type = NUM, .value = {.num = new_node_value}});

            TreeSubTreeDestroy(&node->left);
            TreeSubTreeDestroy(&node->right);
        }
    }
}

void TreeConstConv(Tree* tree) {
    return SubTreeConstConv(TreeNodeGetLeft(TreeGetRoot(tree)));
}

static void SubTreeSimpleOperations(TreeNode* node) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    if (node_value.type == OPERATION) {
        SubTreeSimpleOperations(TreeNodeGetLeft(node));
        SubTreeSimpleOperations(TreeNodeGetRight(node));

        ExpressionElem left_sub_tree = TreeNodeGetValue(TreeNodeGetLeft(node));
        ExpressionElem right_sub_tree = TreeNodeGetValue(TreeNodeGetRight(node));

        if (node_value.value.operation == ADD) {
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 0)) {
                TreeNode node_right = *node->right;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_right.value);
                TreeNodeLinkLeft(node, node_right.left);
                TreeNodeLinkRight(node, node_right.right);
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 0)) {
                TreeNode node_left = *node->left;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_left.value);
                TreeNodeLinkLeft(node, node_left.left);
                TreeNodeLinkRight(node, node_left.right);
            }
        }
        if (node_value.value.operation == SUB) {
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 0)) {
                TreeNode node_left = *node->left;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_left.value);
                TreeNodeLinkLeft(node, node_left.left);
                TreeNodeLinkRight(node, node_left.right);
            }
        }
        if (node_value.value.operation == MUL) {
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 1)) {
                TreeNode node_right = *node->right;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_right.value);
                TreeNodeLinkLeft(node, node_right.left);
                TreeNodeLinkRight(node, node_right.right);
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 1)) {
                TreeNode node_left = *node->left;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_left.value);
                TreeNodeLinkLeft(node, node_left.left);
                TreeNodeLinkRight(node, node_left.right);
            }
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 0)) {
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 0)) {
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
            }
        }
        if (node_value.value.operation == DIV) {
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 1)) {
                TreeNode node_left = *node->left;
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, node_left.value);
                TreeNodeLinkLeft(node, node_left.left);
                TreeNodeLinkRight(node, node_left.right);
            }
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 0)) {
                TreeNodeDestroy(&node->left);
                TreeNodeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
            }
        }
    }
}

void TreeSimpleOperations(Tree* tree) {
    return SubTreeSimpleOperations(TreeNodeGetLeft(TreeGetRoot(tree)));
}

void TreeNodeWithDautherInit(tree_elem_t value, TreeNode* left, TreeNode* right) {
    
}