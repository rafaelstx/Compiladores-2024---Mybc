/*@<parser.h>::*/

extern double T(void);
//extern void R(void);
extern double F(void);
//extern void Q(void);

extern void match(int);

extern FILE *src;

extern int lookahead;
extern int gettoken(FILE *);
extern char lexeme[];