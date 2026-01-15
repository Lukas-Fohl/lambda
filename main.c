#include <stdio.h>
#include <string.h>

#include "util.h"

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

arena* exprArena;

expr* createVal(char* val)
{
    expr* ret = arena_push(exprArena, sizeof(expr));
    ret->type = val_t;
    ret->get.val = val;
    return ret;
}

expr* createFunc(char* arg, expr* body)
{
    expr* ret = arena_push(exprArena, sizeof(expr));
    ret->type = func_t;
    ret->get.func.body = body;
    ret->get.func.arg = arg;
    return ret;
}

expr* createApp(expr* lhs, expr* rhs)
{
    expr* ret = arena_push(exprArena, sizeof(expr));
    ret->type = app_t;
    ret->get.app.lhs = lhs;
    ret->get.app.rhs = rhs;
    return ret;
}

void printExpr(expr* exprIn)
{
    switch (exprIn->type) {
    case val_t:
        printf("%s", exprIn->get.val);
        break;
    case func_t:
        printf("\\%s.", exprIn->get.func.arg);
        printExpr(exprIn->get.func.body);
        break;
    case app_t:
        printf("(");
        printExpr(exprIn->get.app.lhs);
        printf(")");
        printf(" ");
        printExpr(exprIn->get.app.rhs);
        break;
    }
}

// expr* apply(expr* func, expr* arg)
// {
// }

expr* copy(expr* obj)
{
    switch (obj->type) {
    case val_t:
        return createVal(obj->get.val);
    case func_t:
        return createFunc(obj->get.func.arg, copy(obj->get.func.body));
    case app_t:
        return createApp(copy(obj->get.app.lhs), copy(obj->get.app.rhs));
    default:
        return NULL;
    }
}

expr* replace(expr* exprIn, char* name, expr* arg)
{
    switch (exprIn->type) {
    case val_t:
        if (strcmp(exprIn->get.val, name) == 0)
            return arg;
        return exprIn;
    case func_t:
        if (strcmp(exprIn->get.func.arg, name) == 0) {
            return exprIn;
        }
        expr* newBody = replace(exprIn->get.func.body, name, arg);

        if (newBody == exprIn->get.func.body) {
            return exprIn;
        }
        return createFunc(exprIn->get.func.arg, newBody);
    case app_t: {
        expr* newLhs = replace(exprIn->get.app.lhs, name, arg);
        expr* newRhs = replace(exprIn->get.app.rhs, name, arg);

        if (newLhs == exprIn->get.app.lhs && newRhs == exprIn->get.app.rhs) {
            return exprIn;
        }
        return createApp(newLhs, newRhs);
    }
    }
    return NULL;
}

expr* eval(expr* exprIn)
{
    printExpr(exprIn);
    printf("\n");
    switch (exprIn->type) {
    case val_t:
    case func_t:
        return exprIn;

    case app_t: {
        expr* func = eval(exprIn->get.app.lhs);
        expr* arg = eval(exprIn->get.app.rhs);

        if (func->type == func_t) {
            expr* body_copy = copy(func->get.func.body);
            expr* substituted_body = replace(body_copy, func->get.func.arg, arg);
            return eval(substituted_body);
        }

        return createApp(func, arg);
    }
    }
    return NULL;
}

int main(void)
{
    exprArena = arena_alloc(KB_SIZE(5));
    // expr* valA = createVal("a");
    // expr* valB = createVal("b");
    // expr* funcA = createFunc("b", valB);
    // expr* appA = createApp(funcA, valA);
    // printExpr(appA);
    // printf("\n");
    expr* True = createFunc("then", createFunc("else", createVal("then")));
    expr* False = createFunc("then", createFunc("else", createVal("else")));
    expr* t = createApp(createApp(True, createVal("t")), createVal("f"));
    expr* f = createApp(createApp(False, createVal("t")), createVal("f"));
    printExpr(t);
    printf("\n");
    printExpr(eval(t));
    printf("\n");
    printExpr(eval(f));
    free(exprArena);
    return 0;
}
