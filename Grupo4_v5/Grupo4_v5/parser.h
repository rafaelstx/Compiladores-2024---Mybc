#include <stdio.h>
#include <stdlib.h>

extern FILE *src;

extern void match(int);

extern int lookahead;
extern int gettoken(FILE *);
extern char lexeme[];

extern double T(void);
extern double F(void);