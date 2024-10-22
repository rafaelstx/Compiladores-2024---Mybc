#include <stdio.h>

#define MAXLEN 32

enum {
    ID = 1024,
    DEC,
    OCT,
    HEX,
    FLT,
    ASGN, // ASGN = ":="
};

extern int gettoken(FILE *);
extern int linenum;
extern char lexeme[];
