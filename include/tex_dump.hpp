#ifndef TEX_DUMP_HPP_
#define TEX_DUMP_HPP_

#include "tree.hpp"

static const char* TEX_DUMP_BUILD_FILE_NAME = "texdump.tex";
static const char* TEX_DUMP_FILE_NAME = "texdump.pdf";

static const char* FRASES[] = {"Очевидно, что:", "Получим:", "Таким образом:"};

void TexDumpStart();

void CalculatorTreeTexDump(Tree* tree);

void TexDumpEnd();

#endif //TEX_DUMP_HPP_