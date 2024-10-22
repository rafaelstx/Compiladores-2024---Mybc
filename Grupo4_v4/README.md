# CompiladoresMybc
Neste reposítorio estarão disponíveis todas as versões do mybc das aulas de compiladores

Cada versão representa uma mudança,sendo elas:
V1: Adição das função isDec, isHex,isOct e isFloat , retornando se o token é um número decimal,octal, hexadecimal ou float
V2: Adaptar parser para gramática EBNF e Implementada a possibilidade do lexer ignorar comentário Pascal
V3: Implementação do Lexeme, que permite ao compilador não apenas retornar o tipo do token, mas também o conteúdo, que será usado para analisse sintática e semântica no futuro e Adaptação para a gramática EBNF estendida
V4: Implementação do código efetivo na gramática EBNF para análise semântica
