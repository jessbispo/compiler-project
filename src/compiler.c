/*
 * COMPILADOR MINIPASCAL - VERSAO COMPLETA
 * Projeto de Compiladores - Fase 1
 * Análise Lexica e Sintática
 * 
 * Autores: Jessica Bispo, 10410798 e Vitor Alves, [vitor add seu RA aqui]
 * Data: Setembro 2025
 * 
 * Funcionalidades:
 * - Análise Lexica completa com tratamento de erros
 * - Análise Sintática descendente recursiva
 * - Suporte a todas as construções do MiniPascal
 * - Geraçao de arquivo de tokens
 * - Mensagens de erro detalhadas
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#define MAX_LEXEMA 256
#define MAX_PARAMS 50

// tipos de tokens (átomos)
typedef enum {
    ERRO,
    IDENTIFICADOR,
    NUMERO,
    
    // palavras reservadas
    PROGRAM, BEGIN, END, PROCEDURE, FUNCTION,
    IF, THEN, ELSE, WHILE, DO,
    VAR, INTEGER, BOOLEAN,
    TRUE_VAL, FALSE_VAL,
    WRITE, READ,
    AND, OR, NOT, DIV,
    
    // operadores e delimitadores
    MAIS,               // +
    MENOS,              // -
    VEZES,              // *
    MENOR,              // <
    MAIOR,              // >
    IGUAL,              // =
    DIFERENTE,          // <>
    MENOR_IGUAL,        // <=
    MAIOR_IGUAL,        // >=
    ATRIBUICAO,         // :=
    ABRE_PAR,           // (
    FECHA_PAR,          // )
    VIRGULA,            // ,
    PONTO_VIRGULA,      // ;
    DOIS_PONTOS,        // :
    PONTO,              // .
    
    EOS                 // fim do arquivo
} TAtomo;

// estrutura do token
typedef struct {
    TAtomo tipo;
    char lexema[MAX_LEXEMA];
    int linha;
    int valor_inteiro;
} Token;


FILE *arquivo_fonte;
FILE *arquivo_saida;
Token token_atual;
int numero_linha = 1;
char caractere_atual;
int tem_erro = 0;

// vetor de palavras reservadas usado ao inves do automatos
typedef struct {
    const char *palavra;
    TAtomo tipo;
} PalavraReservada;

PalavraReservada palavras_reservadas[] = {
    {"and", AND},
    {"begin", BEGIN},
    {"boolean", BOOLEAN},
    {"div", DIV},
    {"do", DO},
    {"else", ELSE},
    {"end", END},
    {"false", FALSE_VAL},
    {"function", FUNCTION},
    {"if", IF},
    {"integer", INTEGER},
    {"not", NOT},
    {"or", OR},
    {"procedure", PROCEDURE},
    {"program", PROGRAM},
    {"read", READ},
    {"then", THEN},
    {"true", TRUE_VAL},
    {"var", VAR},
    {"while", WHILE}
};

const int NUM_PALAVRAS = sizeof(palavras_reservadas) / sizeof(PalavraReservada);

// func auxiliares

void para_minusculo(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

const char* nome_token(TAtomo tipo) {
    switch(tipo) {
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case PROGRAM: return "program";
        case BEGIN: return "begin";
        case END: return "end";
        case PROCEDURE: return "procedure";
        case FUNCTION: return "function";
        case IF: return "if";
        case THEN: return "then";
        case ELSE: return "else";
        case WHILE: return "while";
        case DO: return "do";
        case VAR: return "var";
        case INTEGER: return "integer";
        case BOOLEAN: return "boolean";
        case TRUE_VAL: return "true";
        case FALSE_VAL: return "false";
        case READ: return "read";
        case WRITE: return "write";
        case AND: return "and";
        case OR: return "or";
        case NOT: return "not";
        case DIV: return "div";
        case MAIS: return "+";
        case MENOS: return "-";
        case VEZES: return "*";
        case MENOR: return "<";
        case MAIOR: return ">";
        case IGUAL: return "=";
        case DIFERENTE: return "<>";
        case MENOR_IGUAL: return "<=";
        case MAIOR_IGUAL: return ">=";
        case ATRIBUICAO: return ":=";
        case ABRE_PAR: return "(";
        case FECHA_PAR: return ")";
        case VIRGULA: return ",";
        case PONTO_VIRGULA: return ";";
        case DOIS_PONTOS: return ":";
        case PONTO: return ".";
        case EOS: return "EOF";
        default: return "DESCONHECIDO";
    }
}

// verifica se um lexema eh uma palavra reservada
int eh_palavra_reservada(const char *lexema, TAtomo *tipo) {
    char lexema_lower[MAX_LEXEMA];
    strcpy(lexema_lower, lexema);
    para_minusculo(lexema_lower);
    
    for (int i = 0; i < NUM_PALAVRAS; i++) {
        if (strcmp(lexema_lower, palavras_reservadas[i].palavra) == 0) {
            *tipo = palavras_reservadas[i].tipo;
            return 1;
        }
    }
    return 0;
}

char proximo_char() {
    char c = fgetc(arquivo_fonte);
    if (c == '\n') {
        numero_linha++;
    }
    return c;
}

void voltar_char() {
    long pos = ftell(arquivo_fonte);
    if (pos > 0) {
        fseek(arquivo_fonte, -1, SEEK_CUR);
        char c = fgetc(arquivo_fonte);
        if (c == '\n') {
            numero_linha--;
        }
        fseek(arquivo_fonte, -1, SEEK_CUR);
    }
}

// analisador lexico ->

void pular_espacos() {
    while (isspace(caractere_atual) && caractere_atual != EOF) {
        caractere_atual = proximo_char();
    }
}


void tratar_comentario() {
    int linha_inicio = numero_linha;
    caractere_atual = proximo_char(); // pula o '/'
    caractere_atual = proximo_char(); // pula o '*'
    
    while (1) {
        if (caractere_atual == EOF) {
            printf("ERRO LEXICO: comentário iniciado na linha %d nao foi fechado\n", linha_inicio);
            tem_erro = 1;
            exit(1);
        }
        
        if (caractere_atual == '*') {
            caractere_atual = proximo_char();
            if (caractere_atual == '/') {
                caractere_atual = proximo_char();
                return;
            }
        } else {
            caractere_atual = proximo_char();
        }
    }
}

Token reconhecer_identificador() {
    Token token;
    int i = 0;
    token.linha = numero_linha;
    
    // Coleta caracteres alfanumericos e underscore
    while ((isalnum(caractere_atual) || caractere_atual == '_') && i < MAX_LEXEMA - 1) {
        token.lexema[i++] = caractere_atual;
        caractere_atual = proximo_char();
    }
    token.lexema[i] = '\0';
    
    // Verifica se e palavra reservada
    if (eh_palavra_reservada(token.lexema, &token.tipo)) {
        fprintf(arquivo_saida, "%d# PALAVRA_RESERVADA | %s\n", token.linha, token.lexema);
        printf("%d# PALAVRA_RESERVADA | %s\n", token.linha, token.lexema);
    } else {
        token.tipo = IDENTIFICADOR;
        fprintf(arquivo_saida, "%d# IDENTIFICADOR | %s\n", token.linha, token.lexema);
        printf("%d# IDENTIFICADOR | %s\n", token.linha, token.lexema);
    }
    
    return token;
}

Token reconhecer_numero() {
    Token token;
    int i = 0;
    token.linha = numero_linha;
    
    while (isdigit(caractere_atual) && i < MAX_LEXEMA - 1) {
        token.lexema[i++] = caractere_atual;
        caractere_atual = proximo_char();
    }
    token.lexema[i] = '\0';
    
    token.tipo = NUMERO;
    token.valor_inteiro = atoi(token.lexema);
    
    fprintf(arquivo_saida, "%d# NUMERO | %s\n", token.linha, token.lexema);
    printf("%d# NUMERO | %s\n", token.linha, token.lexema);
    
    return token;
}


Token criar_token(TAtomo tipo, const char *lexema, int linha) {
    Token token;
    token.tipo = tipo;
    strcpy(token.lexema, lexema);
    token.linha = linha;
    token.valor_inteiro = 0;
    
    fprintf(arquivo_saida, "%d# %s | %s\n", linha, nome_token(tipo), lexema);
    printf("%d# %s | %s\n", linha, nome_token(tipo), lexema);
    
    return token;
}

// obtem o prox token
Token obter_atomo() {
    Token token;
    
    pular_espacos();
    
    if (caractere_atual == EOF) {
        token = criar_token(EOS, "EOF", numero_linha);
        return token;
    }
    
    int linha_token = numero_linha;
    
    if (caractere_atual == '/') {
        char proximo = proximo_char();
        if (proximo == '*') {
            tratar_comentario();
            return obter_atomo(); // recursao para pegar o próximo token
        } else {
            voltar_char();
            printf("ERRO LEXICO: caractere '/' inesperado na linha %d\n", linha_token);
            tem_erro = 1;
            exit(1);
        }
    }
    
    // ===== IDENTIFICADORES e PALAVRAS RESERVADAS =====
    if (isalpha(caractere_atual) || caractere_atual == '_') {
        return reconhecer_identificador();
    }
    
    // ===== NUMEROS =====
    if (isdigit(caractere_atual)) {
        return reconhecer_numero();
    }
    
    // ===== OPERADORES E DELIMITADORES =====
    switch (caractere_atual) {
        case '+':
            token = criar_token(MAIS, "+", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case '-':
            token = criar_token(MENOS, "-", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case '*':
            token = criar_token(VEZES, "*", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case '(':
            token = criar_token(ABRE_PAR, "(", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case ')':
            token = criar_token(FECHA_PAR, ")", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case ',':
            token = criar_token(VIRGULA, ",", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case ';':
            token = criar_token(PONTO_VIRGULA, ";", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case '.':
            token = criar_token(PONTO, ".", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        case ':':
            caractere_atual = proximo_char();
            if (caractere_atual == '=') {
                token = criar_token(ATRIBUICAO, ":=", linha_token);
                caractere_atual = proximo_char();
            } else {
                token = criar_token(DOIS_PONTOS, ":", linha_token);
            }
            return token;
            
        case '<':
            caractere_atual = proximo_char();
            if (caractere_atual == '=') {
                token = criar_token(MENOR_IGUAL, "<=", linha_token);
                caractere_atual = proximo_char();
            } else if (caractere_atual == '>') {
                token = criar_token(DIFERENTE, "<>", linha_token);
                caractere_atual = proximo_char();
            } else {
                token = criar_token(MENOR, "<", linha_token);
            }
            return token;
            
        case '>':
            caractere_atual = proximo_char();
            if (caractere_atual == '=') {
                token = criar_token(MAIOR_IGUAL, ">=", linha_token);
                caractere_atual = proximo_char();
            } else {
                token = criar_token(MAIOR, ">", linha_token);
            }
            return token;
            
        case '=':
            token = criar_token(IGUAL, "=", linha_token);
            caractere_atual = proximo_char();
            return token;
            
        default:
            // Caractere inválido - ERRO LeXICO
            printf("ERRO LeXICO: caractere inválido '%c' (ASCII %d) na linha %d\n", 
                   caractere_atual, (int)caractere_atual, linha_token);
            tem_erro = 1;
            exit(1);
    }
}

// analisador sintatico

// declarações forward (protótipos)
void bloco();
void parte_declaracao_variaveis();
void declaracao_variavel();
void tipo();
void parte_declaracao_subrotinas();
void declaracao_procedimento();
void declaracao_funcao();
void parametros_formais();
void parte_comandos();
void comando();
void comando_atribuicao();
void comando_chamada_proc();
void comando_if();
void comando_while();
void comando_read();
void comando_write();
void comando_composto();
void expressao();
void expressao_simples();
void termo();
void fator();
int eh_operador_relacional();
int eh_operador_adicao();
int eh_operador_multiplicacao();

void consome(TAtomo tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        token_atual = obter_atomo();
    } else {
        printf("\nERRO SINTÁTICO na linha %d:\n", token_atual.linha);
        printf("  Esperado: %s\n", nome_token(tipo_esperado));
        printf("  Encontrado: %s ('%s')\n", nome_token(token_atual.tipo), token_atual.lexema);
        tem_erro = 1;
        exit(1);
    }
}

// <program> ::= program <identifier> ; <block> .
void programa() {
    printf("\n=== INICIANDO ANALISE SINTATICA ===\n\n");
    
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
    
    printf("\n=== ANALISE CONCLUIDA COM SUCESSO! ===\n");
    printf("Programa sintaticamente correto.\n");
}

// <block> ::= <variable declaration part>
//             <subroutine declaration part>
//             <statement part>
void bloco() {
    parte_declaracao_variaveis();
    parte_declaracao_subrotinas();
    parte_comandos();
}

// <variable declaration part> ::= 
//     <empty> | var <variable declaration> ; { var <variable declaration> ; }
void parte_declaracao_variaveis() {
    while (token_atual.tipo == VAR) {
        consome(VAR);
        declaracao_variavel();
        consome(PONTO_VIRGULA);
    }
}

// <variable declaration> ::= <identifier> { , <identifier> } : <type>
void declaracao_variavel() {
    consome(IDENTIFICADOR);
    
    while (token_atual.tipo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    
    consome(DOIS_PONTOS);
    tipo();
}

// <type> ::= integer | boolean
void tipo() {
    if (token_atual.tipo == INTEGER) {
        consome(INTEGER);
    } else if (token_atual.tipo == BOOLEAN) {
        consome(BOOLEAN);
    } else {
        printf("\nERRO SINTÁTICO na linha %d:\n", token_atual.linha);
        printf("  Esperado: tipo (integer ou boolean)\n");
        printf("  Encontrado: %s\n", token_atual.lexema);
        tem_erro = 1;
        exit(1);
    }
}

// <subroutine declaration part> ::= { <procedure declaration> | <function declaration> }
void parte_declaracao_subrotinas() {
    while (token_atual.tipo == PROCEDURE || token_atual.tipo == FUNCTION) {
        if (token_atual.tipo == PROCEDURE) {
            declaracao_procedimento();
        } else {
            declaracao_funcao();
        }
    }
}

// <procedure declaration> ::= procedure <identifier> <formal parameters> ; <block> ;
void declaracao_procedimento() {
    consome(PROCEDURE);
    consome(IDENTIFICADOR);
    
    parametros_formais();
    
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO_VIRGULA);
}

// <function declaration> ::= function <identifier> <formal parameters> : <type> ; <block> ;
void declaracao_funcao() {
    consome(FUNCTION);
    consome(IDENTIFICADOR);
    
    parametros_formais();
    
    consome(DOIS_PONTOS);
    tipo();
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO_VIRGULA);
}

// <formal parameters> ::= ( [ var ] <identifier> : <type> { ; [ var ] <identifier> : <type> } )
void parametros_formais() {
    if (token_atual.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        
        // Parâmetro opcional com var
        if (token_atual.tipo == VAR) {
            consome(VAR);
        }
        
        consome(IDENTIFICADOR);
        consome(DOIS_PONTOS);
        tipo();
        
        // Parâmetros adicionais
        while (token_atual.tipo == PONTO_VIRGULA) {
            consome(PONTO_VIRGULA);
            
            if (token_atual.tipo == VAR) {
                consome(VAR);
            }
            
            consome(IDENTIFICADOR);
            consome(DOIS_PONTOS);
            tipo();
        }
        
        consome(FECHA_PAR);
    }
}

// <statement part> ::= begin <statement> { ; <statement> } end
void parte_comandos() {
    consome(BEGIN);
    
    comando();
    
    while (token_atual.tipo == PONTO_VIRGULA) {
        consome(PONTO_VIRGULA);
        // Permite ponto e vírgula antes do end
        if (token_atual.tipo != END) {
            comando();
        }
    }
    
    consome(END);
}

// <statement> ::= <assignment> | <if> | <while> | <read> | <write> | <compound> | <call>
void comando() {
    if (token_atual.tipo == IDENTIFICADOR) {
        comando_atribuicao();
    } else if (token_atual.tipo == IF) {
        comando_if();
    } else if (token_atual.tipo == WHILE) {
        comando_while();
    } else if (token_atual.tipo == READ) {
        comando_read();
    } else if (token_atual.tipo == WRITE) {
        comando_write();
    } else if (token_atual.tipo == BEGIN) {
        comando_composto();
    } else {
        printf("\nERRO SINTATICO na linha %d:\n", token_atual.linha);
        printf("  Comando invalido: %s\n", token_atual.lexema);
        tem_erro = 1;
        exit(1);
    }
}

// <assignment statement> ::= <variable> := <expression>
// ou <function_procedure statement> ::= <identifier> ( <params> )
void comando_atribuicao() {
    consome(IDENTIFICADOR);
    
    if (token_atual.tipo == ATRIBUICAO) {
        consome(ATRIBUICAO);
        expressao();
    } else if (token_atual.tipo == ABRE_PAR) {
        // Chamada de funçao/procedimento
        consome(ABRE_PAR);
        
        if (token_atual.tipo != FECHA_PAR) {
            // Lista de parâmetros
            expressao();
            
            while (token_atual.tipo == VIRGULA) {
                consome(VIRGULA);
                expressao();
            }
        }
        
        consome(FECHA_PAR);
    }
    // se nao tem := nem (, e apenas um identificador (valido em alguns contextos)
}

// <if statement> ::= if <expression> then <statement> [ else <statement> ]
void comando_if() {
    consome(IF);
    
    if (token_atual.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    } else {
        expressao();
    }
    
    consome(THEN);
    comando();
    
    if (token_atual.tipo == ELSE) {
        consome(ELSE);
        comando();
    }
}

// <while statement> ::= while <expression> do <statement>
void comando_while() {
    consome(WHILE);

    if (token_atual.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    } else {
        expressao();
    }
    
    consome(DO);
    comando();
}

// <read statement> ::= read ( <variable> { , <variable> } )
void comando_read() {
    consome(READ);
    consome(ABRE_PAR);
    consome(IDENTIFICADOR);
    
    while (token_atual.tipo == VIRGULA) {
        consome(VIRGULA);
        consome(IDENTIFICADOR);
    }
    
    consome(FECHA_PAR);
}

// <write statement> ::= write ( <expression> { , <expression> } )
void comando_write() {
    consome(WRITE);
    consome(ABRE_PAR);
    
    expressao();
    
    while (token_atual.tipo == VIRGULA) {
        consome(VIRGULA);
        expressao();
    }
    
    consome(FECHA_PAR);
}

// <compound statement> ::= begin <statement> { ; <statement> } end
void comando_composto() {
    parte_comandos();
}

// verifica se e operador relacional
int eh_operador_relacional() {
    return (token_atual.tipo == MENOR || token_atual.tipo == MAIOR ||
            token_atual.tipo == IGUAL || token_atual.tipo == DIFERENTE ||
            token_atual.tipo == MENOR_IGUAL || token_atual.tipo == MAIOR_IGUAL ||
            token_atual.tipo == AND || token_atual.tipo == OR);
}

// verifica se e operador de adiçao
int eh_operador_adicao() {
    return (token_atual.tipo == MAIS || token_atual.tipo == MENOS || 
            token_atual.tipo == OR);
}

// verifica se e operador de multiplicaçao
int eh_operador_multiplicacao() {
    return (token_atual.tipo == VEZES || token_atual.tipo == DIV || 
            token_atual.tipo == AND);
}

// <expression> ::= <simple expression> [ <relational operator> <simple expression> ]
void expressao() {
    expressao_simples();
    
    if (eh_operador_relacional()) {
        TAtomo op = token_atual.tipo;
        consome(op);
        expressao_simples();
    }
}

// <simple expression> ::= [ <sign> ] <term> { <adding operator> <term> }
void expressao_simples() {
    if (token_atual.tipo == MAIS || token_atual.tipo == MENOS) {
        consome(token_atual.tipo);
    }
    
    termo();
    
    while (eh_operador_adicao()) {
        TAtomo op = token_atual.tipo;
        consome(op);
        termo();
    }
}

// <term> ::= <factor> { <multiplying operator> <factor> }
void termo() {
    fator();
    
    while (eh_operador_multiplicacao()) {
        TAtomo op = token_atual.tipo;
        consome(op);
        fator();
    }
}

// <factor> ::= <variable> | <constant> | ( <expression> ) | not <factor> | <bool>
void fator() {
    if (token_atual.tipo == IDENTIFICADOR) {
        consome(IDENTIFICADOR);
        
        if (token_atual.tipo == ABRE_PAR) {
            consome(ABRE_PAR);
            
            if (token_atual.tipo != FECHA_PAR) {
                expressao();
                
                while (token_atual.tipo == VIRGULA) {
                    consome(VIRGULA);
                    expressao();
                }
            }
            
            consome(FECHA_PAR);
        }
    } else if (token_atual.tipo == NUMERO) {
        consome(NUMERO);
    } else if (token_atual.tipo == TRUE_VAL || token_atual.tipo == FALSE_VAL) {
        consome(token_atual.tipo);
    } else if (token_atual.tipo == ABRE_PAR) {
        consome(ABRE_PAR);
        expressao();
        consome(FECHA_PAR);
    } else if (token_atual.tipo == NOT) {
        consome(NOT);
        fator();
    } else {
        printf("\nERRO SINTÁTICO na linha %d:\n", token_atual.linha);
        printf("  Fator inválido: %s\n", token_atual.lexema);
        tem_erro = 1;
        exit(1);
    }
}


int main(int argc, char *argv[]) {
    printf("╔════════════════════════════════════════════════════════╗\n");
    printf("║        COMPILADOR MINIPASCAL - VERSAO COMPLETA        ║\n");
    printf("║              Análise Lexica e Sintática               ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
    
    if (argc < 2) {
        printf("Uso: %s <arquivo_fonte.pas>\n", argv[0]);
        printf("Exemplo: %s programa.pas\n", argv[0]);
        return 1;
    }
    
    // Abre arquivo fonte
    arquivo_fonte = fopen(argv[1], "r");
    if (!arquivo_fonte) {
        printf("ERRO: Nao foi possível abrir o arquivo '%s'\n", argv[1]);
        printf("Verifique se o arquivo existe e está acessível.\n");
        return 1;
    }
    
    arquivo_saida = fopen("saida/tokens.txt", "w");
    if (!arquivo_saida) {
        printf("ERRO: Nao foi possível criar o arquivo de saida 'tokens.txt'\n");
        fclose(arquivo_fonte);
        return 1;
    }
    
    printf("Arquivo fonte: %s\n", argv[1]);
    printf("Arquivo de tokens: tokens.txt\n\n");
    printf("=== INICIANDO ANÁLISE LeXICA ===\n\n");
    
    //inicializo
    caractere_atual = proximo_char();
    
    // obtem o primeiro token
    token_atual = obter_atomo();
    
    printf("\n");
    
    // inicia analise sintatica
    programa();
    
    // fecha arquivos
    fclose(arquivo_fonte);
    fclose(arquivo_saida);
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║                  COMPILAÇAO CONCLUIDA                  ║\n");
    printf("║           Arquivo de tokens gerado com sucesso         ║\n");
    printf("╚════════════════════════════════════════════════════════╝\n");
    
    return 0;
}
