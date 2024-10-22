/*@<lexer.c>::*/

#include <stdio.h>
#include <ctype.h>
#include <lexer.h>
#include <string.h>
#include <tokens.h>
#define MAXLEN 64
char lexeme[MAXLEN+1];




int isASGN(FILE *tape)
{
	if ( (lexeme[0] = getc(tape)) == ':' ) {
		if ( (lexeme[1] = getc(tape) ) == '=' ) {
			lexeme[2] = 0;
			return ASGN;
		}
		ungetc(lexeme[1], tape);
	}
	ungetc(lexeme[0], tape);
	return 0;
}
/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */

int isID(FILE *tape)
{
	int i=0;

	lexeme[i] = getc(tape);

	if (isalpha(lexeme[i])) {
		i = 1;
		while(isalnum(lexeme[i] = getc(tape))) 
			if (i<MAXLEN) i++;
		
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;

		//Print teste
		//printf ("%s ", lexeme);
		if ((strcmp(lexeme,"quit") == 0) || (strcmp(lexeme,"QUIT") == 0)){
			return QUIT;
		}
		if ((strcmp(lexeme,"exit") == 0) ||(strcmp(lexeme,"EXIT") == 0) ){
			return EXIT;
		}
		return ID;
	}
	
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;
	return 0;
}

/*
 * OCT = 0[0-7]
 */
int isOCTAL(FILE *tape)
{
	int i=0;
	lexeme[i] = getc(tape);

	if(lexeme[i] != '0'){ 
		ungetc(lexeme[i], tape); 
		lexeme[i]=0;
		return 0; 
	}

	i++;

	lexeme[i] = getc(tape);
	if(lexeme[i]<'0' || lexeme[i]>'7' || !(isdigit(lexeme[i]))){
		ungetc(lexeme[i], tape);
		i--;
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;

		return 0;
	}

	i++;

	while((lexeme[i] = getc(tape)) && (lexeme[i]>='0') && (lexeme[i] <= '7')){
		if (i<MAXLEN) i++;
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;

	//Print teste
	//printf ("%s ", lexeme);
	return OCT;
	
}

/*
 * HEX = [1-9][0-9]* | 0
 */
int isHEX(FILE *tape)
{
	int x, i=0;

	lexeme[i] = getc(tape);
	if(lexeme[i]!='0'){
		ungetc(lexeme[i], tape);
		return 0;	
	}
	i++;

	lexeme[i] = getc(tape);
	if((lexeme[i] != 'X') && (lexeme[i] != 'x')){
		ungetc(lexeme[i],tape);
		i--;
		ungetc(lexeme[i], tape);

		return 0;
	}

	x = lexeme[i];
	i++;

	lexeme[i] = getc(tape);
	if(!(isxdigit(lexeme[i]))){
		ungetc(lexeme[i], tape);
		i--;
		ungetc(lexeme[i], tape);
		i--;
		ungetc(lexeme[i], tape);

		return 0;
	}

	i++;
	while(isxdigit(lexeme[i] = getc(tape))){
		if (i<MAXLEN) i++;
	}
	ungetc(lexeme[i], tape);
	lexeme[i] = 0;

	//Print teste
	//printf ("%s", lexeme);
	
	return HEX;

}
/* EE = ee [′+′′ − ′]? [0 − 9]+.

*/
int isEE(FILE *tape){
	int i = 0; 
	
	i = strlen(lexeme);

	char e;
	lexeme[i] = getc(tape);
	if(lexeme[i] != 'E' && lexeme[i] != 'e'){
		ungetc(lexeme[i], tape);
		lexeme[i] = 0;
		return 0;
	}
	
	e = lexeme[i];
	i++;
	lexeme[i] = getc(tape);

	char sinal = ' ';
	if(lexeme[i] == '-' || lexeme[i] == '+'){
		sinal = lexeme[i];
		i++;
		lexeme[i] = getc(tape);
	}
	if(!isdigit(lexeme[i])){ 
		ungetc(lexeme[i], tape);
		i--;
		if (sinal != ' ')
			//ungetc(sinal, tape);
			ungetc(lexeme[i], tape);
			i--;
		//ungetc(e, tape);
		ungetc(lexeme[i], tape);

		lexeme[i] = 0;
		return 0;
	}	
	i++;
	while((lexeme[i] = getc(tape)) && isdigit(lexeme[i])){
		if (i<MAXLEN) i++;
	}

	ungetc(lexeme[i], tape);
	lexeme[i]=0;
	return EE;
	
}

/*
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	int i=0; 
	lexeme[i] = getc(tape);

	if (isdigit(lexeme[i])) {
		if (lexeme[i] == '0') {
			lexeme[i] = 0;
			return DEC;
		}
		i++;

		while(isdigit(lexeme[i] = getc(tape))){
			if (i<MAXLEN) i++;
		}
		
		ungetc(lexeme[i], tape);
		lexeme[i] =0;
		return DEC;
	}
	
	ungetc(lexeme[i], tape);
	lexeme[i]=0;
	return 0;
}

/*
 * FLT = (DEC '.' [0-9]* | '.' [0-9]+) EE ? | DEC EE
 */
int isFLOAT(FILE *tape)
{
	int i=0;
	lexeme[i] = getc(tape);

	if(lexeme[i] == '.'){
		i++;
		if (isdigit(lexeme[i] = getc(tape))){
			i++;
			while(isdigit(lexeme[i] = getc(tape))){
				i++;
			}

			ungetc(lexeme[i],tape);
			i--;
			
			if (isEE(tape) == EE);
			i = strlen(lexeme);

			lexeme[i] = 0;
			//printf("%s ", lexeme);
			
			return FLT;
		}
		else {
			ungetc(lexeme[i],tape);
			i--;
			ungetc(lexeme[i], tape);

			lexeme[i] = 0;
			return 0;
		}

	}
	else{
		ungetc(lexeme[i],tape);
		i--;
		if(isDEC(tape) == DEC){
			if( isEE(tape) == EE){
				//printf("%s ", lexeme);
				return FLT;
			}
			
			i=strlen(lexeme);
			

			lexeme[i] = getc(tape);

			if (lexeme[i] == '.'){
				i++;
				while(isdigit(lexeme[i] = getc(tape))){i++;};
				ungetc(lexeme[i],tape);
				lexeme[i]=0;

				if(isEE(tape) == EE);

				i=strlen(lexeme);
				lexeme[i]=0;

				//Print teste
				//printf("%s ", lexeme);

				return FLT;
		
			}
			else{
				ungetc(lexeme[i],tape);

				//Print teste
				//printf("%s ", lexeme);

				return DEC;
			}
		}
		else {
			return 0;
		}
	}

	
}
int isNUM(FILE *source){
	int token;
	//if ( (token = isEE(source))) return token;
	if ( (token = isOCTAL(source))) return token;
	if ( (token = isHEX(source))) return token;
	if ( (token = isFLOAT(source))) return token;
	return 0;
}

/*** Evitem o uso explícito de constantes. Deem preferência a EOF do que -1 ***/
void skipspaces(FILE *tape)
{
	verifica:

	int head;

	while(isspace(head = getc(tape)));

	if(head == '{'){
		while(head != '}' && head != -1){
			head = getc(tape);
		}
		goto verifica;
	} else{
		ungetc(head,tape);
	}

		
}

int gettoken(FILE *source)
{
	//printf("%s\n\n", lexeme);
	int token;
	skipspaces(source);
	
	if ( (token = isNUM(source))) return token;
	if ( (token = isID(source)) ) return token;
	if ( (token = isASGN(source)) ) return token;
	/*if ( (token = isEE(source)) ) return token;
	if ( (token = isOCTAL(source)) ) return token;
	if ( (token = isHEX(source)) ) return token;
	if ( (token = isFLOAT(source)) ) return token;
	//if ( (token = isDEC(source)) ) return token;
	*/
	return getc(source);
}
