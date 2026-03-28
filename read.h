#ifndef READ_H
#define READ_H

#include "expr.h"
#include "util.h"

typedef struct nameExpr {
    char* name;
    expr* expr;
} nameExpr;

DYNAMIC_LIST_PROTOTYPE(nameExpr)
DYNAMIC_LIST_IMPL(nameExpr)


void populateStd();
bool isInStd(char* nameIn);
nameExpr getByName(char* nameIn);
expr* replaceFromStd(expr* origin);
expr* stringToExpr(char* input);

#endif
