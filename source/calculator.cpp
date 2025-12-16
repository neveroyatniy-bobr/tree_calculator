#include "calculator.hpp"

#include <stdio.h>
#include <math.h>
#include <memory.h>

#include "expression_elem.hpp"
#include "tree.hpp"
#include "utils.hpp"

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
                return MUL(POW(SUB(CONST(1), POW(cL, CONST(2))), CONST(-1/2)), dL);
            case COS:
                return MUL(MUL(CONST(-1), SIN(cL)), dL);
            case ARCCOS:
                return MUL(CONST(-1), MUL(POW(SUB(CONST(1), POW(cL, CONST(2))), CONST(-1/2)), dL));
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

static void CalculatorSubTreeTexDump(TreeNode* node, FILE* build_file) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    tree_elem_t node_parent_value = TreeNodeGetValue(TreeNodeGetParent(node));
    if (node_value.type == OPERATION) {
        switch (node_value.value.operation) {
            case ADD:
                if ((node_parent_value.type == OPERATION && node_parent_value.value.operation == MUL) 
                 || (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW)) {
                    fprintf(build_file, "(");
                }

                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, " + ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);

                if ((node_parent_value.type == OPERATION && node_parent_value.value.operation == MUL) 
                 || (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW)) {
                    fprintf(build_file, ")");
                }

                return;
            case SUB:
                if ((node_parent_value.type == OPERATION && node_parent_value.value.operation == MUL) 
                 || (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW)) {
                    fprintf(build_file, "(");
                }

                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, " - ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);

                if ((node_parent_value.type == OPERATION && node_parent_value.value.operation == MUL) 
                 || (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW)) {
                    fprintf(build_file, ")");
                }

                return;
            case MUL:
                if (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW) {
                    fprintf(build_file, "(");
                }

                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, " \\cdot ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);

                if (node_parent_value.type == OPERATION && node_parent_value.value.operation == POW) {
                    fprintf(build_file, ")");
                }

                return;
            case DIV:
                fprintf(build_file, "\\frac{");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, "}{");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);
                fprintf(build_file, "}");

                return;
            case POW:
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, "^{");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);
                fprintf(build_file, "}");

                return;
            case SIN:
                fprintf(build_file, "\\sin(");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, ")");

                return;
            case ARCSIN:
                fprintf(build_file, "\\arcsin(");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, ")");

                return;
            case COS:
                fprintf(build_file, "\\cos(");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, ")");

                return;
            case ARCCOS:
                fprintf(build_file, "\\arccos(");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, ")");

                return;
            case LN:
                fprintf(build_file, "\\ln(");
                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, ")");

                return;
            default:
                return;
        }
    }

    if (node_value.type == VAR) {
        switch (node_value.value.var)
        {
        case X:
            fprintf(build_file, "x");
            return;
        case Y:
            fprintf(build_file, "y");
            return;
        default:
            return;
        }
    }
    
    if (node_value.value.num < 0) {
        fprintf(build_file, "(");
    }

    fprintf(build_file, "%.3lf", node_value.value.num);

    if (node_value.value.num < 0) {
        fprintf(build_file, ")");
    }
}

void TexDumpStart() {
    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "w");

    const char* file_start = "\\documentclass{article}\n\\usepackage{graphicx}\n\\usepackage[T2A]{fontenc}\n\\title{dump}\n\\author{Николай Антипов}\n\\date{November 2025}\n\\begin{document}\n";

    fprintf(build_file, "%s", file_start);

    fclose(build_file);
}

void CalculatorTreeTexDump(Tree* tree) {
    static size_t line_num = 1;

    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "a");

    if (line_num != 1) {
        size_t frases_count = sizeof(FRASES) / sizeof(FRASES[0]);
        fprintf(build_file, "%s\\newline\n", FRASES[RandInt(frases_count - 1)]);
    }

    fprintf(build_file, "$");
    
    CalculatorSubTreeTexDump(TreeNodeGetLeft(TreeGetRoot(tree)), build_file);

    fprintf(build_file, "$\\newline\n");

    fclose(build_file);

    line_num++;
}

void TexDumpEnd() {
    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "a");

    const char* file_end = "\\end{document}\n";

    fprintf(build_file, "%s", file_end);

    fclose(build_file);

    char command[BUILD_DUMP_COMMAND_SIZE + 1] = "";
    snprintf(command, BUILD_DUMP_COMMAND_SIZE, "pdflatex %s 12> /dev/null", TEX_DUMP_BUILD_FILE_NAME);

    system(command);
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

void TreeSimplify(Tree* tree) {
    size_t last_tree_size = TreeGetSize(tree);
    TreeConstConv(tree);
    TreeSimpleOperations(tree);
    size_t curr_tree_size = TreeGetSize(tree);

    while (last_tree_size != curr_tree_size) {
        CalculatorTreeTexDump(tree);
        last_tree_size = curr_tree_size;
        TreeConstConv(tree);
        TreeSimpleOperations(tree);

        curr_tree_size = TreeGetSize(tree);
    }
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