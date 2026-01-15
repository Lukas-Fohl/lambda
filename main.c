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
        exprIn->get.func.body = replace(exprIn->get.func.body, name, arg);
        return exprIn;
    case app_t:
        exprIn->get.app.lhs = replace(exprIn->get.app.lhs, name, arg);
        exprIn->get.app.rhs = replace(exprIn->get.app.rhs, name, arg);
        return exprIn;
    }
}

expr* eval(expr* exprIn)
{
    printExpr(exprIn);
    printf("\n");
    switch (exprIn->type) {
    case val_t:
    case func_t:
        return exprIn;
    case app_t:
        if (exprIn->get.app.lhs->type == func_t) {
            exprIn->get.app.lhs = eval(exprIn->get.app.lhs);
            return replace(
                exprIn->get.app.lhs->get.func.body,
                exprIn->get.app.lhs->get.func.arg,
                eval(exprIn->get.app.rhs));
        } else if (exprIn->get.app.lhs->type == app_t) {
            exprIn->get.app.lhs = eval(exprIn->get.app.lhs);
            return eval(exprIn);
        } else {
            printf("well fuck\n");
            abort();
        }
        return NULL;
    }
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
