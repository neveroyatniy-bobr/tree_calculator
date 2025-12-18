#include "tex_dump.hpp"

#include <stdio.h>

#include "utils.hpp"

static bool IsOp(TreeNode* node, Operation op) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    return (node_value.type == OPERATION && node_value.value.operation == op);
}

static void CalculatorSubTreeTexDump(TreeNode* node, FILE* build_file) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    tree_elem_t node_parent_value = TreeNodeGetValue(TreeNodeGetParent(node));
    if (node_value.type == OPERATION) {
        switch (node_value.value.operation) {
            case ADD:
                if (IsOp(TreeNodeGetParent(node), MUL) || IsOp(TreeNodeGetParent(node), POW)) {
                    fprintf(build_file, "(");
                }

                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, " + ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);

                if (IsOp(TreeNodeGetParent(node), MUL) || IsOp(TreeNodeGetParent(node), POW)) {
                    fprintf(build_file, ")");
                }

                return;
            case SUB:
                if (IsOp(TreeNodeGetParent(node), MUL) || IsOp(TreeNodeGetParent(node), POW)) {
                    fprintf(build_file, "(");
                }

                CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);
                fprintf(build_file, " - ");
                CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);

                if (IsOp(TreeNodeGetParent(node), MUL) || IsOp(TreeNodeGetParent(node), POW)) {
                    fprintf(build_file, ")");
                }

                return;
            case MUL:
                if (IsOp(TreeNodeGetParent(node), POW)) {
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

    fprintf(build_file, "%g", node_value.value.num);

    if (node_value.value.num < 0) {
        fprintf(build_file, ")");
    }
}

void TexDumpStart() {
    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "w");

    const char* file_start = "\\documentclass{article}\n"
                             "\\usepackage{graphicx}\n"
                             "\\usepackage[T2A]{fontenc}\n"
                             "\\title{dump}\n"
                             "\\author{Николай Антипов}\n"
                             "\\date{November 2025}\n"
                             "\\begin{document}\n";

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
    snprintf(command, BUILD_DUMP_COMMAND_SIZE, "pdflatex %s 2&1> /dev/null", TEX_DUMP_BUILD_FILE_NAME);

    if (system(command) != 0) {
        fprintf(stderr, "Ошибка при создании файла дампа");
    }
}