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

    expr* OrExpr = createFunc("p",
        createFunc("q",
            createApp(
                createApp(createVal("q"), createVal("p")),
                createVal("p"))));
    nameExpr OrNameExpr = { .name = "Or", .expr = OrExpr };
    dyn_nameExpr_push(&std, OrNameExpr);

    expr* NotExpr = createFunc("p",createApp(createApp(createVal("p"), FalseExpr), TrueExpr));
    nameExpr NotNameExpr = { .name = "Not", .expr = NotExpr };
    dyn_nameExpr_push(&std, NotNameExpr);

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

extern arena* exprArena;

void skipWhiteSpace(char** input)
{
    while (**input != '\0' && (**input == ' ' || **input == '\n' || **input == '\t' || **input == '\r')) {
        (*input)++;
    }
}

char* parseVar(char** input)
{
    skipWhiteSpace(input);
    char* start = *input;
    while (**input != '\0' && ((**input >= 'a' && **input <= 'z') || (**input >= 'A' && **input <= 'Z') || (**input >= '0' && **input <= '9'))) {
        (*input)++;
    }
    size_t len = *input - start;
    if (len == 0)
        return NULL;
    char* name = arena_push(exprArena, len + 1);
    memcpy(name, start, len);
    name[len] = '\0';
    return name;
}

expr* parseExpr(char** input);

expr* parseAtom(char** input)
{
    skipWhiteSpace(input);
    if (**input == '(') {
        (*input)++;
        expr* e = parseExpr(input);
        skipWhiteSpace(input);
        if (**input != ')') {
            giveUp("Expected )");
        }
        (*input)++;
        return e;
    } else {
        char* name = parseVar(input);
        if (!name)
            return NULL;
        if(isInStd(name)) {
            return copy(getByName(name).expr);
        }
        return createVal(name);
    }
}

expr* parseApp(char** input)
{
    expr* left = parseAtom(input);
    if (!left)
        return NULL;

    while (1) {
        skipWhiteSpace(input);
        // Application terminates on end of string, closing paren, or start of a new lambda
        if (**input == '\0' || **input == ')' || **input == '\\') {
            break;
        }
        expr* right = parseAtom(input);
        if (!right)
            break;
        left = createApp(left, right);
    }
    return left;
}

expr* parseExpr(char** input)
{
    skipWhiteSpace(input);
    if (**input == '\\') {
        (*input)++;
        char* name = parseVar(input);
        if (!name) {
            giveUp("cannot read func parameter name");
        }
        skipWhiteSpace(input);
        if (**input != '.') {
            giveUp("Expected .");
        }
        (*input)++;
        expr* body = parseExpr(input);
        if (!body) {
            giveUp("Expected body after .");
        }
        return createFunc(name, body);
    } else {
        return parseApp(input);
    }
}

expr* stringToExpr(char* input)
{
    char* ptr = input;
    expr* result = parseExpr(&ptr);
    skipWhiteSpace(&ptr);
    if (*ptr != '\0') {
        giveUp("Trailing characters after expression");
    }
    return result;
}

