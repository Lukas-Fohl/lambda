#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "expr.h"
#include "read.h"
#include "util.h"

extern arena* exprArena;

int main(void)
{
    exprArena = arena_alloc(KB_SIZE(5));

    expr* True = createFunc("then", createFunc("else", createVal("then")));
    printExpr(True);
    printf("\n");
    // expr* Id = createFunc("x", createVal("x"));
    expr* False = createFunc("then", createFunc("else", createVal("else")));
    printExpr(False);
    printf("\n");
    expr* And = createFunc("p",
        createFunc("q",
            createApp(
                createApp(createVal("p"), createVal("q")),
                createVal("p"))));
    expr* a = createApp(createApp(And, True), False);

    expr* t = createApp(createApp(True, createVal("t")), createVal("f"));
    expr* f = createApp(createApp(False, createVal("t")), createVal("f"));
    printExpr(t);
    printf("\n");
    printExpr(eval(t));
    printf("\n");
    printExpr(eval(f));

    printf("And True False\n");
    printExpr(a);
    printf("\n");
    printExpr(eval(a));

    populateStd();
    printf("\n");
    printf("some diff");
    printf("\n");
    expr* test = createApp(createApp(createVal("And"), createVal("True")), createVal("True"));
    printExpr(eval(replaceFromStd(test)));
    printf("\n");

    // expr* alpha = createApp(createFunc("x", createFunc("y", createVal("x"))), createVal("y"));
    // printf("\n");
    // printExpr(alpha);
    // printf("\n");
    // printExpr(eval(alpha));

    // expr* rec = createApp(createFunc("x",createApp(createVal("x"),createVal("x"))),createFunc("x",createApp(createVal("x"),createVal("x"))));
    // printf("\n");
    // printExpr(rec);
    // printf("\n");
    // printExpr(eval(rec));
    // free(exprArena);

    return 0;
}

/*
TODO:
    - [x] alpha renaming
    - eval step -> check recursion
    - lexer
 */
