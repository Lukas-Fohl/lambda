#include "expr.h"
#include "util.h"

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

char* getNewName()
{
    static u32 counter = 0;
    char* str = arena_push(exprArena, 16 * sizeof(char));
    sprintf(str, "FR%uEE", counter++);
    return str;
}

bool containsFree(expr* exprIn, char* nameIn)
{
    switch (exprIn->type) {
    case val_t:
        return (strcmp(exprIn->get.val, nameIn) == 0);
    case app_t:
        return containsFree(exprIn->get.app.lhs, nameIn) || containsFree(exprIn->get.app.rhs, nameIn);
    case func_t:
        if (strcmp(exprIn->get.func.arg, nameIn) == 0) {
            return false;
        }
        return containsFree(exprIn->get.func.body, nameIn);
    }
    return false;
}

void printExpr(expr* exprIn)
{
    switch (exprIn->type) {
    case val_t:
        printf("%s", exprIn->get.val);
        break;
    case func_t:
        printf("λ%s.", exprIn->get.func.arg);
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

        if (containsFree(arg, exprIn->get.func.arg)) {
            char* newName = getNewName();
            expr* newBody = replace(exprIn->get.func.body, exprIn->get.func.arg, createVal(newName));
            expr* newFunc = createFunc(newName, newBody);
            return replace(newFunc, name, arg);
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
#ifdef DEBUG
    printExpr(exprIn);
    printf("\n");
#endif
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

bool equal(expr* lhs, expr* rhs)
{
    printf("pls don't use this for equal %d\n", __LINE__);
    if (lhs->type != rhs->type)
        return false;
    switch (lhs->type) {
    case val_t:
        return strcmp(lhs->get.val, rhs->get.val) == 0;
    case func_t:
        return strcmp(lhs->get.func.arg, rhs->get.func.arg) == 0
            && equal(lhs->get.func.body, rhs->get.func.body);
    case app_t:
        return equal(lhs->get.app.lhs, rhs->get.app.lhs)
            && equal(lhs->get.app.rhs, rhs->get.app.rhs);
    default:
        return false;
    }
}
