#include <stdio.h>
#include <ctype.h>
#include <lexer.h>
#include <string.h>
#include <tokens.h>

int isASGN(FILE *tape)
{
	char first = getc(tape);
	if (first == ':') {
		char second = getc(tape);
		if (second == '=') {
			return ASGN;
		}
		ungetc(second, tape);
	}
	ungetc(first, tape);
	return 0;
}

/*
 * ID = [A-Za-z][A-Za-z0-9]*
 */
int isID(FILE *tape)
{
	char first = getc(tape);
	if (isalpha(first)) {
		char next;
		while (isalnum(next = getc(tape)));  // Consome os caracteres válidos sem armazená-los
		ungetc(next, tape);
		return ID;
	}
	ungetc(first, tape);
	return 0;
}

/*
 * OCT = 0[0-7]
 */
int isOCTAL(FILE *tape)
{
	char first = getc(tape);
	if (first != '0') {
		ungetc(first, tape);
		return 0;
	}

	char second = getc(tape);
	if (second < '0' || second > '7') {
		ungetc(second, tape);
		ungetc(first, tape);
		return 0;
	}

	char next;
	while ((next = getc(tape)) >= '0' && next <= '7');  // Consome caracteres de octal
	ungetc(next, tape);
	return OCT;
}

/*
 * HEX = 0x[0-9A-Fa-f]+
 */
int isHEX(FILE *tape)
{
	char first = getc(tape);
	if (first != '0') {
		ungetc(first, tape);
		return 0;
	}

	char second = getc(tape);
	if (second != 'X' && second != 'x') {
		ungetc(second, tape);
		ungetc(first, tape);
		return 0;
	}

	char next;
	if (!isxdigit(next = getc(tape))) {
		ungetc(next, tape);
		ungetc(second, tape);
		ungetc(first, tape);
		return 0;
	}

	while (isxdigit(next = getc(tape)));  // Consome caracteres hexadecimais
	ungetc(next, tape);
	return HEX;
}

/*
 * EE = [Ee][+-]?[0-9]+
 */
int isEE(FILE *tape)
{
	char e = getc(tape);
	if (e != 'E' && e != 'e') {
		ungetc(e, tape);
		return 0;
	}

	char sign = getc(tape);
	if (sign == '-' || sign == '+') {
		sign = getc(tape);
	}

	if (!isdigit(sign)) {
		ungetc(sign, tape);
		ungetc(e, tape);
		return 0;
	}

	char next;
	while (isdigit(next = getc(tape)));  // Consome dígitos
	ungetc(next, tape);
	return EE;
}

/*
 * DEC = [1-9][0-9]* | 0
 */
int isDEC(FILE *tape)
{
	char first = getc(tape);
	if (isdigit(first)) {
		if (first == '0') {
			return DEC;
		}

		char next;
		while (isdigit(next = getc(tape)));  // Consome dígitos
		ungetc(next, tape);
		return DEC;
	}

	ungetc(first, tape);
	return 0;
}

/*
 * FLT = (DEC '.' [0-9]* | '.' [0-9]+) EE? | DEC EE
 */
int isFLOAT(FILE *tape)
{
	char first = getc(tape);
	if (first == '.') {
		char next = getc(tape);
		if (isdigit(next)) {
			while (isdigit(next = getc(tape)));  // Consome dígitos após o ponto
			ungetc(next, tape);
			if (isEE(tape)) return FLT;
			return FLT;
		}
		ungetc(next, tape);
		ungetc(first, tape);
		return 0;
	} else {
		ungetc(first, tape);
		if (isDEC(tape)) {
			if (isEE(tape)) return FLT;
			char next = getc(tape);
			if (next == '.') {
				while (isdigit(next = getc(tape)));  // Consome dígitos após o ponto
				ungetc(next, tape);
				if (isEE(tape)) return FLT;
				return FLT;
			}
			ungetc(next, tape);
			return DEC;
		}
	}
	return 0;
}

int isNUM(FILE *source) {
	int token;
	if ((token = isOCTAL(source))) return token;
	if ((token = isHEX(source))) return token;
	if ((token = isFLOAT(source))) return token;
	return 0;
}

/*** Evitem o uso explícito de constantes. Deem preferência a EOF do que -1 ***/
void skipspaces(FILE *tape)
{
	int head;
	while (isspace(head = getc(tape)));

	if (head == '{') {
		while (head != '}' && head != EOF) {
			head = getc(tape);
		}
		skipspaces(tape);
	} else {
		ungetc(head, tape);
	}
}

int gettoken(FILE *source)
{
	int token;
	skipspaces(source);

	if ((token = isNUM(source))) return token;
	if ((token = isID(source))) return token;
	if ((token = isASGN(source))) return token;
	
	return getc(source);
}
