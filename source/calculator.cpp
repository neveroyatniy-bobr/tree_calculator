#include "calculator.hpp"

#include <stdio.h>
#include <math.h>
#include <memory.h>

#include "expression_elem.hpp"
#include "tree.hpp"

//FIXME хочу чтобы сам читал количество членов enum
static double var[2] = {1, 1};

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
            case POW:
                return pow(left_sub_tree_value, right_sub_tree_value);
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
            case ADD:
                return ADD(dL, dR);
            case SUB:
                return SUB(dL, dR);
            case MUL:
                return ADD(MUL(dL, cR), MUL(cL, dR));
            case DIV:
                return DIV(SUB(MUL(dL, cR), MUL(cL, dR)), MUL(cR, cR));
            case POW:
                return MUL(POW(cL, cR), ADD(MUL(DIV(dL, cL), cR), MUL(LN(cL), dR)));
            case SIN:
                return MUL(COS(cL), dL);
            case ARCSIN:
            case COS:
                return MUL(MUL(CONST(-1), SIN(cL)), dL);
            case ARCCOS:
            case LN:
                return MUL(DIV(CONST(1), cL), dL);
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
            case POW: {
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf("^{");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node));
                printf("}");

                printf(")");
            }
            case LN: {
                printf("\\ln{");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node));
                printf("}");

                printf(")");
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
                TreeSubTreeDestroy(&node->left);
                TreeSubTreeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 0)) {
                TreeSubTreeDestroy(&node->left);
                TreeSubTreeDestroy(&node->right);

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
                TreeSubTreeDestroy(&node->left);
                TreeSubTreeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
            }
        }
        if (node_value.value.operation == POW) {
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 0)) {
                if (IsEqual(CalculateSubTree(TreeNodeGetRight(node)), 0)) {
                    TreeSubTreeDestroy(&node->left);
                    TreeSubTreeDestroy(&node->right);
    
                    TreeNodeSetValue(node, {.type = NUM, .value = {.num = 1}});
                }
                else {
                    TreeSubTreeDestroy(&node->left);
                    TreeSubTreeDestroy(&node->right);
    
                    TreeNodeSetValue(node, {.type = NUM, .value = {.num = 0}});
                }
            }
            if (left_sub_tree.type == NUM && IsEqual(left_sub_tree.value.num, 1)) {
                TreeSubTreeDestroy(&node->left);
                TreeSubTreeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 1}});
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 0)) {
                TreeSubTreeDestroy(&node->left);
                TreeSubTreeDestroy(&node->right);

                TreeNodeSetValue(node, {.type = NUM, .value = {.num = 1}});
            }
            if (right_sub_tree.type == NUM && IsEqual(right_sub_tree.value.num, 1)) {
                TreeSubTreeDestroy(&node->right);
                TreeNode* node_left = node->left;

                TreeNodeSetValue(node, left_sub_tree);
                TreeNodeLinkLeft(node, TreeNodeGetLeft(node_left));
                TreeNodeLinkRight(node, TreeNodeGetRight(node));

                TreeNodeLinkLeft(node_left, NULL);
                TreeNodeLinkRight(node_left, NULL);
                TreeSubTreeDestroy(&node_left);
            }
        }
    }
}

void TreeSimpleOperations(Tree* tree) {
    return SubTreeSimpleOperations(TreeNodeGetLeft(TreeGetRoot(tree)));
}

TreeNode* TreeNodeWithDautherInit(tree_elem_t value, TreeNode* left, TreeNode* right) {
    TreeNode* node = (TreeNode*)calloc(1, sizeof(TreeNode));

    if (node == NULL) {
        return NULL;
    }

    node->parent = NULL;

    TreeNodeLinkLeft(node, left);
    TreeNodeLinkRight(node, right);

    node->value = value;

    return node;
}