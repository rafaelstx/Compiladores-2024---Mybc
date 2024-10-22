#include <stdio.h>         
#include <stdlib.h>       
#include <string.h>       
#include "lexer.h"        
#include "parser.h"       

int lookahead;            // Variável para armazenar o próximo token a ser analisado

// Definições de constantes para tamanhos máximos
#define MAXSTACKSIZE 1024
#define MAXSYMTABSIZE 1024

// Declaração de variáveis globais
double Acc, stack[MAXSTACKSIZE];          // A pilha para armazenar valores
int sp = -1;                               // Índice da pilha (stack pointer)
char symtab[MAXSYMTABSIZE][MAXLEN + 1];  // Tabela de símbolos para variáveis
double vm[MAXSYMTABSIZE];                  // Valores das variáveis
int symtab_next = 0;                       // Índice para a próxima posição na tabela de símbolos

/**
 * Adiciona um valor à pilha.
 * @param x O valor a ser empilhado.
 */
void Push(double x) {
    if (sp < MAXSTACKSIZE - 1) { // Verifica se há espaço na pilha
        stack[++sp] = x;         // Adiciona o valor à pilha
    }
}

/**
 * Remove e retorna o valor do topo da pilha.
 * @return O valor do topo da pilha ou 0.0 se a pilha estiver vazia.
 */
double Pop(void) {
    return (sp >= 0) ? stack[sp--] : 0.0;  // Retorna 0.0 se a pilha estiver vazia
}

/**
 * Recupera o valor de uma variável da tabela de símbolos.
 * Se a variável não for encontrada, ela é adicionada com o valor 0.0.
 * @param varname O nome da variável a ser recuperada.
 * @return O valor da variável.
 */
double recall(char *varname) {
    for (int i = 0; i < symtab_next; i++) { // Busca na tabela de símbolos
        if (strcmp(symtab[i], varname) == 0) { // Se a variável é encontrada
            return vm[i];                      // Retorna o valor correspondente
        }
    }
    strcpy(symtab[symtab_next], varname);     // Adiciona a variável à tabela
    symtab_next++;
    return 0.0;                               // Retorna 0.0 se não encontrada
}

/**
 * Armazena um valor na tabela de símbolos.
 * Se a variável já existir, atualiza seu valor.
 * @param varname O nome da variável a ser armazenada.
 * @param x O valor a ser armazenado.
 */
void store(char *varname, double x) {
    for (int i = 0; i < symtab_next; i++) { // Busca na tabela de símbolos
        if (strcmp(symtab[i], varname) == 0) { // Se a variável é encontrada
            vm[i] = x;                        // Atualiza o valor
            return;
        }
    }
    strcpy(symtab[symtab_next], varname);     // Adiciona nova variável
    vm[symtab_next++] = x;                     // Armazena seu valor
}

/**
 * Executa uma operação aritmética.
 * @param op O operador ( +, -, *, / )
 * @param op1 O primeiro operando.
 * @param op2 O segundo operando.
 * @return O resultado da operação.
 */
double execop(int op, double op1, double op2) {
    switch (op) {
        case '+': return op1 + op2;  // Adição
        case '-': return op1 - op2;  // Subtração
        case '*': return op1 * op2;  // Multiplicação
        case '/': return op1 / op2;  // Divisão
        default: return 0.0;         // Retorna 0.0 para operação inválida
    }
}

/*
 * Produção E da gramática:
 * E →  [-]T{+ T} | [+]T{+ T} | [-]T{- T} | [+]T{- T}
 * Analisa expressões e retorna seu valor.
 */
double E(void) {
    int signal = (lookahead == '-' || lookahead == '+') ? lookahead : 0; // Verifica sinal
    if (signal) match(signal);  // Se houver sinal, consome o token

    double T_val = T();         // Obtém o valor da expressão T
    if (signal == '-') T_val = -T_val; // Se sinal negativo, inverte o valor

    double E_val = T_val;       // Inicializa E_val com o valor de T
    while (lookahead == '+' || lookahead == '-') { // Enquanto houver operadores
        int oplus = lookahead;   // Armazena o operador
        match(oplus);            // Consome o operador
        T_val = T();            // Obtém o próximo valor T
        E_val = execop(oplus, E_val, T_val); // Executa a operação
    }
    return E_val;               // Retorna o valor final da expressão
}

/**
 * Analisa um comando. Se for um comando válido, avalia e imprime o resultado.
 */
void cmd(void) {
    if (lookahead == ';' || lookahead == '\n' || lookahead == EOF) {
        // Não faz nada se for apenas um delimitador
    } else {
        double E_val = E();       // Avalia a expressão
        printf("%lg\n", E_val);   // Imprime o resultado
    }
}

/**
 * Função principal do interpretador.
 * Executa comandos enquanto houver tokens válidos.
 */
void mybc(void) {
    cmd(); // Avalia o primeiro comando
    while (lookahead == ';' || lookahead == '\n') { // Enquanto houver delimitadores
        match(lookahead); // Consome os delimitadores
        cmd();            // Avalia o próximo comando
    }
    match(EOF); // Consome o final do arquivo
}

/*
 * Produção T da gramática:
 * T → F{* F}| F{/ F}
 * Analisa termos e retorna seu valor.
 */
double T(void) {
    double T_val = F(); // Obtém o valor do primeiro fator
    while (lookahead == '*' || lookahead == '/') { // Enquanto houver multiplicação ou divisão
        int otimes = lookahead; // Armazena o operador
        match(otimes);          // Consome o operador
        double F_val = F();     // Obtém o próximo fator
        T_val = execop(otimes, T_val, F_val); // Executa a operação
    }
    return T_val; // Retorna o valor final do termo
}

/*
 * Produção F da gramática:
 * F → ID | DEC | ( E )
 * Analisa fatores e retorna seu valor.
 */
double F(void) {
    double F_val = 0.0; // Inicializa o valor do fator
    char name[MAXLEN + 1]; // Buffer para o nome da variável

    switch (lookahead) {
        case '(': // Se for um parêntese
            match('('); // Consome o parêntese
            F_val = E(); // Avalia a expressão dentro do parêntese
            match(')'); // Consome o parêntese de fechamento
            break;
        case OCT:
        case HEX:
        case DEC:
        case FLT:
            F_val = atof(lexeme); // Converte o número para double
            match(lookahead); // Consome o token numérico
            break;
        default: // Se for uma variável
            strcpy(name, lexeme); // Armazena o nome da variável
            match(ID); // Consome o token de ID
            if (lookahead == ASGN) { // Se houver um operador de atribuição
                match(ASGN); // Consome o operador
                F_val = E(); // Avalia a expressão
                store(name, F_val); // Armazena o resultado na tabela de símbolos
            } else {
                F_val = recall(name); // Recupera o valor da variável
            }
    }
    return F_val; // Retorna o valor final do fator
}

/**
 * Consome um token esperado.
 * Se o token atual não corresponder ao esperado, exibe uma mensagem de erro e encerra o programa.
 * @param expected O token esperado.
 */
void match(int expected) {
    if (lookahead == expected) {
        lookahead = gettoken(src); // Consome o token esperado
    } else {
        fprintf(stderr, "incompatibilidade de token! esperado "); // Mensagem de erro
        if (33 <= expected && expected <= 126) {
            fprintf(stderr, "%c", expected); // Mostra o token esperado
        } else if (expected >= ID && expected <= FLT) {
            fprintf(stderr, "numero"); // Indica que era esperado um número
        }
        fprintf(stderr, "%c\n", lookahead); // Mostra o token recebido
        exit(-2); // Encerra o programa com erro
    }
}
