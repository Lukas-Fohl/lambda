#include <string.h>

#include "util.h"
#include "read.h"

dyn_nameExpr std;

void populateStd()
{
    std = dyn_nameExpr_create(20);

    expr* TrueExpr = createFunc("then", createFunc("else", createVal("then")));
    nameExpr TrueNameExpr = { .name = "True", .expr = TrueExpr };
    dyn_nameExpr_push(&std, TrueNameExpr);

    expr* FalseExpr = createFunc("then", createFunc("else", createVal("else")));
    nameExpr FalseNameExpr = { .name = "False", .expr = FalseExpr };
    dyn_nameExpr_push(&std, FalseNameExpr);

    expr* AndExpr = createFunc("p",
        createFunc("q",
            createApp(
                createApp(createVal("p"), createVal("q")),
                createVal("p"))));
    nameExpr AndNameExpr = { .name = "And", .expr = AndExpr };
    dyn_nameExpr_push(&std, AndNameExpr);

    return;
}

bool isInStd(char* nameIn)
{
    for (unsigned i = 0; i < dyn_nameExpr_len(&std); i++) {
        if (strcmp(dyn_nameExpr_get(&std, i).name, nameIn) == 0) {
            return true;
        }
    }
    return false;
}

nameExpr getByName(char* nameIn)
{
    for (unsigned i = 0; i < dyn_nameExpr_len(&std); i++) {
        if (strcmp(dyn_nameExpr_get(&std, i).name, nameIn) == 0) {
            return dyn_nameExpr_get(&std, i);
        }
    }
    giveUp("cannot find requested");
}

expr* replaceFromStd(expr* origin)
{
    for (unsigned i = 0; i < dyn_nameExpr_len(&std); i++) {
        origin = replace(origin,
            dyn_nameExpr_get(&std, i).name,
            dyn_nameExpr_get(&std, i).expr);
    }
    return origin;
}

expr* stringToExpr(char* input){
    return NULL;
}
