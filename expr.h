#ifndef EXPR_H
#define EXPR_H

#include <string.h>
#include <stdbool.h>

typedef enum {
    val_t = 0,
    func_t,
    app_t
} exprType;

typedef struct expr expr;

struct expr {
    exprType type;
    union {
        char* val;
        struct {
            expr* lhs;
            expr* rhs;
        } app;
        struct {
            char* arg;
            expr* body;
        } func;
    } get;
};

expr* createVal(char* val);
expr* createFunc(char* arg, expr* body);
expr* createApp(expr* lhs, expr* rhs);
char* getNewName();
bool containsFree(expr* exprIn, char* nameIn);
void printExpr(expr* exprIn);
expr* copy(expr* obj);
expr* replace(expr* exprIn, char* name, expr* arg);
expr* eval(expr* exprIn);
bool equal(expr* lhs, expr* rhs);

#endif
