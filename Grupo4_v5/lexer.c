#include <stdio.h>
#include <ctype.h>
#include "lexer.h"
#include <string.h>

#define MAXLEN 32 // Define o tamanho máximo do lexema
char lexeme[MAXLEN + 1]; // Array para armazenar o lexema atual

/*
 * Função: isASGN
 * Descrição: Verifica se os próximos caracteres correspondem ao operador de atribuição ":="
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - ASGN se o operador de atribuição for reconhecido, 0 caso contrário.
 */
int isASGN(FILE *tape) {
    if ((lexeme[0] = getc(tape)) == ':') {
        if ((lexeme[1] = getc(tape)) == '=') {
            lexeme[2] = 0; // Termina a string do lexema
            return ASGN; // Retorna o token de atribuição
        }
        ungetc(lexeme[1], tape); // Coloca de volta o último caractere lido
    }
    ungetc(lexeme[0], tape); // Coloca de volta o caractere lido anteriormente
    return 0; // Não reconhecido
}

/*
 * Função: isID
 * Descrição: Verifica se os próximos caracteres correspondem a um identificador (ID).
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - ID se um identificador for reconhecido, 0 caso contrário.
 */
int isID(FILE *tape) {
    lexeme[0] = getc(tape); // Lê o primeiro caractere

    if (isalpha(lexeme[0])) { // Verifica se é letra
        int i = 1;
        while (isalnum(lexeme[i] = getc(tape))) { // Lê caracteres alfanuméricos
            i++;
        }

        ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
        lexeme[i] = 0; // Termina a string do lexema

        return ID; // Retorna o token ID
    }

    ungetc(lexeme[0], tape); // Coloca de volta o caractere lido
    lexeme[0] = 0; // Reseta o lexema

    return 0; // Não reconhecido
}

/*
 * Função: isDEC
 * Descrição: Verifica se os próximos caracteres correspondem a um número decimal (DEC).
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - DEC se um número decimal for reconhecido, 0 caso contrário.
 */
int isDEC(FILE *tape) {
    int i = 0;
    lexeme[i] = getc(tape); // Lê o primeiro caractere

    if (isdigit(lexeme[i])) { // Verifica se é um dígito
        if (lexeme[i] == '0') { // Se for zero, é um número decimal
            lexeme[i] = 0;
            return DEC;
        }
        i++;

        while (isdigit(lexeme[i] = getc(tape))) { // Continua lendo enquanto for dígito
            if (i < MAXLEN) i++;
        }

        ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
        lexeme[i] = 0; // Termina a string do lexema
        return DEC; // Retorna o token DEC
    }

    ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
    lexeme[i] = 0; // Reseta o lexema
    return 0; // Não reconhecido
}

/*
 * Função: isOCTAL
 * Descrição: Verifica se os próximos caracteres correspondem a um número octal (OCT).
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - OCT se um número octal for reconhecido, 0 caso contrário.
 */
int isOCTAL(FILE *tape) {
    int i = 0;
    lexeme[i] = getc(tape); // Lê o primeiro caractere

    if (lexeme[i] != '0') { // O primeiro caractere deve ser '0' para octal
        ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
        lexeme[i] = 0;
        return 0; // Não reconhecido
    }

    i++;
    lexeme[i] = getc(tape); // Lê o próximo caractere
    if (lexeme[i] < '0' || lexeme[i] > '7' || !isdigit(lexeme[i])) { // Verifica se está entre '0' e '7'
        ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
        i--;
        ungetc(lexeme[i], tape); // Coloca de volta o '0'
        lexeme[i] = 0;
        return 0; // Não reconhecido
    }

    i++;

    // Continua lendo enquanto for um dígito octal
    while ((lexeme[i] = getc(tape)) && (lexeme[i] >= '0') && (lexeme[i] <= '7')) {
        if (i < MAXLEN) i++;
    }
    ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
    lexeme[i] = 0; // Termina a string do lexema

    return OCT; // Retorna o token OCT
}

/*
 * Função: isHEX
 * Descrição: Verifica se os próximos caracteres correspondem a um número hexadecimal (HEX).
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - HEX se um número hexadecimal for reconhecido, 0 caso contrário.
 */
int isHEX(FILE *tape) {
    int i = 0;

    lexeme[i] = getc(tape); // Lê o primeiro caractere
    if (lexeme[i] != '0') { // O primeiro caractere deve ser '0'
        ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
        return 0; // Não reconhecido
    }
    i++;

    lexeme[i] = getc(tape); // Lê o próximo caractere
    if (lexeme[i] != 'X' && lexeme[i] != 'x') { // Verifica se é 'X' ou 'x'
        ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
        i--;
        ungetc(lexeme[i], tape); // Coloca de volta o '0'
        return 0; // Não reconhecido
    }
    i++;

    // Continua lendo enquanto for um dígito hexadecimal
    while (isxdigit(lexeme[i] = getc(tape))) {
        if (i < MAXLEN) i++;
    }
    ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
    lexeme[i] = 0; // Termina a string do lexema

    return HEX; // Retorna o token HEX
}

/*
 * Função: isFLOAT
 * Descrição: Verifica se os próximos caracteres correspondem a um número flutuante (FLT).
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - FLT se um número flutuante for reconhecido, 0 caso contrário.
 */
int isFLOAT(FILE *tape) {
    int i = 0;
    lexeme[i] = getc(tape); // Lê o primeiro caractere

    if (lexeme[i] == '.') { // Se o primeiro caractere for um ponto
        i++;
        if (isdigit(lexeme[i] = getc(tape))) { // Verifica se o próximo caractere é um dígito
            i++;
            while (isdigit(lexeme[i] = getc(tape))) { // Lê os dígitos após o ponto
                i++;
            }

            ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
            lexeme[i] = 0; // Termina a string do lexema
            return FLT; // Retorna o token FLT
        } else {
            ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
            ungetc(lexeme[i - 1], tape); // Coloca de volta o ponto
            lexeme[i] = 0; // Reseta o lexema
            return 0; // Não reconhecido
        }
    } else {
        ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
        if (isDEC(tape) == DEC) { // Verifica se é um número decimal
            i = strlen(lexeme); // Armazena o tamanho do lexema
            lexeme[i] = getc(tape); // Lê o próximo caractere
            if (lexeme[i] == '.') { // Se for um ponto, continua
                i++;
                while (isdigit(lexeme[i] = getc(tape))) { i++; }; // Lê os dígitos após o ponto
                ungetc(lexeme[i], tape); // Coloca de volta o último caractere lido
                lexeme[i] = 0; // Termina a string do lexema

                return FLT; // Retorna o token FLT
            } else {
                ungetc(lexeme[i], tape); // Coloca de volta o caractere lido
                return DEC; // Retorna o token DEC
            }
        }
        return 0; // Não reconhecido
    }
}

/*
 * Função: isNUM
 * Descrição: Verifica se os próximos caracteres correspondem a um número (decimal, octal, hexadecimal ou flutuante).
 * Parâmetro: 
 *   - FILE *source: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - Um dos tokens: OCT, HEX, FLT ou 0 caso contrário.
 */
int isNUM(FILE *source) {
    int token;
    if ((token = isOCTAL(source))) return token; // Verifica octal
    if ((token = isHEX(source))) return token; // Verifica hexadecimal
    if ((token = isFLOAT(source))) return token; // Verifica flutuante
    return 0; // Não reconhecido
}

int linenum = 1; // Contador de linhas para rastreamento de erros

/*
 * Função: skipspaces
 * Descrição: Ignora espaços em branco e conta linhas.
 * Parâmetro: 
 *   - FILE *tape: Ponteiro para o arquivo de entrada.
 */
void skipspaces(FILE *tape) {
    int head;

    // Continua lendo enquanto for um espaço em branco
    while (isspace(head = getc(tape))) {
        if (head == '\n') // Conta novas linhas
            linenum++;
    }

    ungetc(head, tape); // Coloca de volta o último caractere lido
}

/*
 * Função: gettoken
 * Descrição: Obtém o próximo token do arquivo de entrada, ignorando espaços em branco.
 * Parâmetro: 
 *   - FILE *source: Ponteiro para o arquivo de entrada.
 * Retorno: 
 *   - O próximo token reconhecido.
 */
int gettoken(FILE *source) {
    int token;
    skipspaces(source); // Ignora espaços em branco

    // Verifica os diferentes tipos de tokens
    if ((token = isNUM(source))) return token; // Verifica se é um número
    if ((token = isID(source))) return token; // Verifica se é um identificador
    if ((token = isASGN(source))) return token; // Verifica se é um operador de atribuição

    return getc(source); // Retorna o próximo caractere se nada for reconhecido
}
