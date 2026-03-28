#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "expr.h"
#include "read.h"
#include "util.h"

#define MAX_CHAR_READ_LIMIT 1024

extern arena* exprArena;

int main(void)
{
    exprArena = arena_alloc(KB_SIZE(500));

    populateStd();

    char cmdInput[MAX_CHAR_READ_LIMIT];

    char* helpString = "Included functions are: True, False, And, Or, Not\nUse q to quit\nUse h for help";
    printf("%s\n", helpString);

    while (1) {
        printf(">>> ");
        if (fgets(cmdInput, MAX_CHAR_READ_LIMIT, stdin) == NULL) {
            break;
        }

        cmdInput[strcspn(cmdInput, "\n")] = '\0';

        if (strcmp(cmdInput, "q") == 0) {
            break;
        } else if (strcmp(cmdInput, "h") == 0) {
            printf("%s\n", helpString);
        }

        if (strlen(cmdInput) == 0) {
            continue;
        }
        printExpr(eval(stringToExpr(cmdInput)));
        printf("\n");
    }

    free(exprArena);

    return 0;
}
