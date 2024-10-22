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
 *
 * LL(1)-grammar for simple expressions - initial symbol: E
 * E -> T R
 * R -> + T R | - T R | <epsilon>
 * T -> F Q
 * Q -> * F Q | / F Q | <epsilon>
 * F -> ID | DEC | ( E )
 */

/**/
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
char symtab[MAXSYMTABSIZE][MAXLEN+1];
int symtab_next;
double vm[MAXSYMTABSIZE];
double Recall(char *varname)
{
	int i;
	for (i = 0; i < symtab_next; i++) {
		if (strcmp(symtab[i], varname) == 0) {
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
	for (i = 0; i < symtab_next; i++) {
		if (strcmp(symtab[i], varname) == 0) break;
	}
	if (i == symtab_next) {
		strcpy(symtab[i], varname);
		symtab_next++;
	}
	vm[i] = x;
}


double execop(int op, double op1, double op2){
	switch(op){
		case '+':
			return op1 + op2;
			break;

		case '-': 
			return op1 - op2;
			break;

		case '*':
			return op1 * op2;
			break;

		case '/':
			return op1 / op2;
			break;
	}
}


/* E →  [-]T{+ T} | [+]T{+ T} | [-]T{- T} | [+]T{- T}*/
double E(void) {

	int signal = 0; double T_val, E_val;
	if (lookahead == '-'|| lookahead == '+') {

		signal = lookahead;
		match(signal);

	}

	T_val = T();
	if (signal == '-') {
		T_val = -T_val;

	}
	E_val = T_val;

	_T:
	if (lookahead == '+' || lookahead == '-'){
		int oplus = lookahead;
		match(oplus); 
		T_val = T();
		E_val = execop(oplus, E_val, T_val);
		goto _T;
	}

		

	return E_val;

}
/**/

void cmd(void){
	double E_val;
	switch (lookahead){
		case ';':
		case '\n':
		case EOF:
			/*Nao faz nada*/
		case QUIT:
		case EXIT:
			exit(0);
		default:
			E_val = E();
			printf ("%lg\n", E_val);
	}
}

/*** Como mybc abstrai o símbolo inicial da gramática final, este deveria ser o primeiro precedimento implementado
***/
void mybc(void){

	cmd();
	while (lookahead == ';' || lookahead == '\n'){
		match(lookahead);
		cmd();
	}
	match(EOF);
}



/* T → F{* F}| F{/ F}*/
double T(void) {

	double T_val, F_val;

	
	T_val = F();
	_F:
	if (lookahead == '*'|| lookahead =='/') {
		int otimes = lookahead;
		match(otimes); 
		F_val = F();

		T_val = execop(otimes, T_val, F_val);
		//printf ("T val   %lg\tF Val  %lg",T_val,F_val);
		goto _F;
	}

	return T_val;

}

/*** Deveriam ter marcado as ações semânticas. Quase não vi as ações das constantes ***/
/* F -> ID | DEC | ( E ) */
double F(void) {

	double F_val; 
	char name[MAXLEN+1];
	switch(lookahead) {
		case '(':
			match('('); 
			F_val = E(); 
			match(')'); 
			break;

		case OCT:
			F_val = atof(lexeme);
			match(OCT);
			break;
		case HEX:
			F_val = atof(lexeme);
			match(HEX);
			break;
		case DEC:
			F_val = atof(lexeme);
			match(DEC);
			break;
		case FLT:
			F_val =	 atof(lexeme);
			match(FLT); 	  
			break;

		default:

			strcpy(name, lexeme);
			match(ID);
			if (lookahead == ASGN) {
				match(ASGN); 
				F_val = E();
				Store(name, F_val);

			} else {

				F_val = Recall(name);

			}

		}
	return F_val;

}

void match(int expected)
{
	if (lookahead == expected) {
		lookahead = gettoken(src);
	} else {
		fprintf(stderr, "token mismatch! expected ");
		if (33 <= expected && expected <=126){
			fprintf(stderr,"%c", expected);
		}else if(expected >= ID && expected <=FLT){
			fprintf (stderr,"number ");
		}
		if (33 <= expected && expected <=126){
			fprintf(stderr,"and had %c\n", lookahead);
		}else {
			fprintf (stderr,"and had %c instead\n", lookahead);
		}

		exit(-2);
	}
}
