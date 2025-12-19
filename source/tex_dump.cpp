#include "tex_dump.hpp"

#include <stdio.h>

#include "utils.hpp"

static bool IsOp(TreeNode* node, Operation op) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    return (node_value.type == OPERATION && node_value.value.operation == op);
}

struct OpDump {
    Operation op;
    size_t priority;
    bool is_bin;
    const char* prefix;
    const char* infix;
    const char* postfix;
};

//Нужно в порядке объявления в enum Operation
OpDump OP_DUMPS[] = { 
    {    ADD, 3,  true, "", " + ", ""        },
    {    SUB, 3,  true, "", " - ", ""        },
    {    MUL, 2,  true, "", " \\cdot ", ""   },
    {    DIV, 2,  true, "\\frac{", "}{", "}" },
    {    POW, 1,  true, "", "^{", "}"        },
    {    SIN, 0, false, "\\sin(", "", ")"    },
    { ARCSIN, 0, false, "\\arcsin(", "", ")" },
    {    COS, 0, false, "\\cos(", "", ")"    },
    { ARCCOS, 0, false, "\\arccos(", "", ")" },
    {     LN, 0, false, "\\ln(", "", ")"     }
};
const size_t op_dumps_cnt = sizeof(OP_DUMPS) / sizeof(OP_DUMPS[0]);

static void CalculatorSubTreeTexDump(TreeNode* node, FILE* build_file) {
    tree_elem_t node_value = TreeNodeGetValue(node);
    tree_elem_t node_parent_value = TreeNodeGetValue(TreeNodeGetParent(node));
    if (node_value.type == OPERATION) {
        OpDump op_dump = OP_DUMPS[node_value.value.operation];
        bool is_brackets = false;
        if (node_parent_value.type == OPERATION) {
            size_t parent_priority = (OP_DUMPS[node_parent_value.value.operation]).priority;
            if (op_dump.priority > parent_priority) {
                is_brackets = true;
            }
        }

        if (is_brackets) {
            fprintf(build_file, "(");
        }

        fprintf(build_file, "%s", op_dump.prefix);

        CalculatorSubTreeTexDump(TreeNodeGetLeft(node), build_file);

        if (op_dump.is_bin) {
            fprintf(build_file, "%s", op_dump.infix);

            CalculatorSubTreeTexDump(TreeNodeGetRight(node), build_file);
        }

        fprintf(build_file, "%s", op_dump.postfix);

        if (is_brackets) {
            fprintf(build_file, ")");
        }

        return ;
    }

    if (node_value.type == VAR) {
        const char* var_str = "unknown";
        switch (node_value.value.var)
        {
            case X:  var_str = "x";       break;
            case Y:  var_str = "y";       break;
            default: var_str = "unknown"; break;
        }
        fprintf(build_file, "%s", var_str);
        return ;
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
                             "\\linespread{2}"
                             "\\begin{document}\n"
                             "\\begin{center}\n";

    fprintf(build_file, "%s", file_start);  

    fclose(build_file);
}

void CalculatorTreeTexDump(Tree* tree) {
    static size_t line_num = 1;

    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "a");

    fprintf(build_file, "\\textbf{%d)", line_num);

    if (line_num != 1) {
        size_t frases_count = sizeof(FRASES) / sizeof(FRASES[0]);
        fprintf(build_file, "%s", FRASES[RandInt(frases_count - 1)]);
    }
    else {
        fprintf(build_file, "Давайте возьмем производную от данного выражения:");
    }

    fprintf(build_file, "}\\newline\n$");
    
    CalculatorSubTreeTexDump(TreeNodeGetLeft(TreeGetRoot(tree)), build_file);

    fprintf(build_file, "$\\newline\n");

    fclose(build_file);

    line_num++;
}

void TexDumpEnd() {
    FILE* build_file = fopen(TEX_DUMP_BUILD_FILE_NAME, "a");

    const char* file_end = "\\end{center}"
                           "\\end{document}\n";

    fprintf(build_file, "%s", file_end);

    fclose(build_file);

    char command[BUILD_DUMP_COMMAND_SIZE + 1] = "";
    snprintf(command, BUILD_DUMP_COMMAND_SIZE, "pdflatex %s > /dev/null", TEX_DUMP_BUILD_FILE_NAME);

    if (system(command) != 0) {
        fprintf(stderr, "Ошибка при создании файла дампа");
    }
}