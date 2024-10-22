/*@<parser.c>::*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <lexer.h>
#include <parser.h>

int lookahead;

/* LR(1)-grammar for simple expressions - initial symbol: E
 *
 * E -> E + T | E - T | T
 * T -> T * F | T / F | F
 * F -> ID | DEC | ( E )
 */

/** Stack and symbol table management **/
#define MAXSTACKSIZE 1024
double Acc, stack[MAXSTACKSIZE];
int sp = -1;
void Push(double x)
{
    sp++;
    stack[sp] = x;
}
double Pop(void)
{
    double y = stack[sp];
    sp--;
    return y;
}
#define MAXSYMTABSIZE 16384
char symtab[MAXSYMTABSIZE][MAXLEN + 1];
int symtab_next;
double vm[MAXSYMTABSIZE];

double Recall(char *varname)
{
    int i;
    for (i = 0; i < symtab_next; i++)
    {
        if (strcmp(symtab[i], varname) == 0)
        {
            return vm[i];
        }
    }
    strcpy(symtab[i], varname);
    symtab_next++;
    return 0.0;
}

void Store(char *varname, double x)
{
    int i;
    for (i = 0; i < symtab_next; i++)
    {
        if (strcmp(symtab[i], varname) == 0)
            break;
    }
    if (i == symtab_next)
    {
        strcpy(symtab[i], varname);
        symtab_next++;
    }
    vm[i] = x;
}

double execop(int op, double op1, double op2)
{
    switch (op)
    {
    case '+':
        return op1 + op2;
    case '-':
        return op1 - op2;
    case '*':
        return op1 * op2;
    case '/':
        return op1 / op2;
    default:
        fprintf(stderr, "Invalid operator\n");
        exit(1);
    }
}

/* E -> E + T | E - T | T */
double E(void)
{
    double T_val, E_val;
    T_val = T();
    E_val = T_val;

    while (lookahead == '+' || lookahead == '-')
    {
        int oplus = lookahead;
        match(oplus);
        T_val = T();
        E_val = execop(oplus, E_val, T_val);
    }

    return E_val;
}

/* T -> T * F | T / F | F */
double T(void)
{
    double T_val, F_val;
    T_val = F();

    while (lookahead == '*' || lookahead == '/')
    {
        int otimes = lookahead;
        match(otimes);
        F_val = F();
        T_val = execop(otimes, T_val, F_val);
    }

    return T_val;
}

/* F -> ID | DEC | ( E ) */
double F(void)
{
    double F_val;
    char name[MAXLEN + 1];

    switch (lookahead)
    {
    case '(':
        match('(');
        F_val = E();
        match(')');
        break;
    case DEC:
        F_val = atof(lexeme);
        match(DEC);
        break;
    case FLT:
        F_val = atof(lexeme);
        match(FLT);
        break;
    case ID:
        strcpy(name, lexeme);
        match(ID);
        if (lookahead == ASGN)
        {
            match(ASGN);
            F_val = E();
            Store(name, F_val);
        }
        else
        {
            F_val = Recall(name);
        }
        break;
    default:
        fprintf(stderr, "Syntax error: unexpected token in F\n");
        exit(1);
    }

    return F_val;
}

void match(int expected)
{
    if (lookahead == expected)
    {
        lookahead = gettoken(src);
    }
    else
    {
        fprintf(stderr, "Token mismatch! Expected %c, got %c\n", expected, lookahead);
        exit(1);
    }
}

/* Main command function */
void cmd(void)
{
    double E_val;
    switch (lookahead)
    {
    case ';':
    case '\n':
    case EOF:
        /* No operation */
        break;
    case QUIT:
    case EXIT:
        exit(0);
    default:
        E_val = E();
        printf("%lg\n", E_val);
        break;
    }
}

/* Main entry point for the calculator */
void mybc(void)
{
    lookahead = gettoken(src);
    cmd();
    while (lookahead == ';' || lookahead == '\n')
    {
        match(lookahead);
        cmd();
    }
    match(EOF);
}
