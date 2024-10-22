#include <stdio.h>
#include <stdlib.h>
#include "main.h"

FILE *src; // Ponteiro para o arquivo de entrada

/*
 * Função: main
 * Descrição: Função principal do programa que gerencia a entrada de dados e inicia a análise.
 * Parâmetros:
 *   - int argc: Número de argumentos da linha de comando.
 *   - char *argv[]: Array de strings contendo os argumentos da linha de comando.
 * Retorno: 
 *   - int: Código de saída do programa (0 em caso de sucesso).
 */
int main(int argc, char *argv[]) {
    // Verifica se foi passado um argumento (nome do arquivo) na linha de comando
    if (argc > 1) {
        src = fopen(argv[1], "r"); // Tenta abrir o arquivo para leitura
        if (!src) { // Se não conseguiu abrir o arquivo
            fprintf(stderr, "Nao foi possivel encontrar o arquivo\n"); // Exibe mensagem de erro
            return EXIT_FAILURE; // Retorna código de erro
        }
    } else {
        src = stdin; // Se nenhum arquivo for passado, lê da entrada padrão (stdin)
    }
    
    lookahead = gettoken(src); // Obtém o primeiro token da entrada
    mybc(); // Chama a função de análise (ou processamento) principal

    return EXIT_SUCCESS; // Retorna 0 para indicar sucesso
}
