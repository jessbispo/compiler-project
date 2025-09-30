# 📚 COMPILADOR MINIPASCAL - DOCUMENTAÇÃO COMPLETA

**Projeto de Compiladores - Fase 1**  
Análise Léxica e Sintática

---

## 📋 ÍNDICE

1. [Visão Geral](#visão-geral)
2. [Estrutura do Projeto](#estrutura-do-projeto)
3. [Como Compilar e Executar](#como-compilar-e-executar)
4. [Arquivos de Teste](#arquivos-de-teste)
5. [Funcionalidades Implementadas](#funcionalidades-implementadas)
6. [Como Funciona](#como-funciona)
7. [Tratamento de Erros](#tratamento-de-erros)
8. [Exemplos de Uso](#exemplos-de-uso)
9. [Decisões de Design](#decisões-de-design)
10. [Bugs Conhecidos](#bugs-conhecidos)

---

## 🎯 VISÃO GERAL

Este é um compilador completo para a linguagem **MiniPascal**, implementado em **C**, que realiza:

- ✅ **Análise Léxica**: Identifica todos os tokens (palavras reservadas, identificadores, números, operadores, delimitadores)
- ✅ **Análise Sintática**: Verifica se o programa segue a gramática da linguagem
- ✅ **Tratamento de Comentários**: Ignora comentários /* */
- ✅ **Detecção de Erros**: Reporta erros léxicos e sintáticos com linha e descrição
- ✅ **Geração de Arquivo de Tokens**: Cria arquivo tokens.txt com todos os tokens identificados

---

## 📁 ESTRUTURA DO PROJETO

```
projeto_compilador/
│
├── src:
│   ├──compiler.c              # Código fonte do compilador
│
├── test:
│   ├── teste_basico.pas         # Programa simples
│   ├── teste_completo.pas       # Programa do enunciado
│   ├── teste_if_while.pas       # Testa estruturas de controle
│   ├── teste_expressoes.pas     # Testa expressões complexas
│   ├── teste_procedimentos.pas  # Testa procedures e functions
│   ├── teste_comentarios.pas    # Testa comentários
│   ├── teste_erro_lexico.pas    # Gera erro léxico
│   └── teste_erro_sintatico.pas # Gera erro sintático
│
├── README.txt                   # Esta documentação
│
└── out:
    ├── compilador               # Executável (Linux/Mac)
    └── tokens.txt               # Arquivo de saída com tokens
```

---

## 🔨 COMO COMPILAR E EXECUTAR

### No Linux/Mac

1. **Compilar**:
```bash
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
```

2. **Executar**:
```bash
./compilador arquivo-pascal-de-teste.pas
```


---

## 🧪 ARQUIVOS DE TESTE

### 1. teste_basico.pas
**O que testa**: Programa mínimo válido
```pascal
program teste;
var x : integer;
begin
    x := 10;
end.
```

**Uso**: `././compilador testes/testes/teste_basico.pas`

---

### 2. teste_completo.pas
**O que testa**: Programa completo com procedures, if, while, expressões
- Declaração de variáveis múltiplas
- Procedures com parâmetros
- Comandos if e while aninhados
- Operações aritméticas e lógicas

**Uso**: `./compilador testes/teste_completo.pas`

---

### 3. teste_if_while.pas
**O que testa**: Estruturas de controle
- Loop while
- Comando if-then-else
- Comentários
- Operadores relacionais

**Uso**: `./compilador testes/teste_if_while.pas`

---

### 4. teste_expressoes.pas
**O que testa**: Expressões complexas
- Expressões aritméticas: +, -, *, div
- Precedência de operadores
- Parênteses
- Expressões booleanas: and, or, not
- Operadores relacionais: <, >, <=, >=, =, <>

**Uso**: `./compilador testes/teste_expressoes.pas`

---

### 5. teste_procedimentos.pas
**O que testa**: Procedures e functions
- Declaração de procedures
- Parâmetros com var
- Functions com retorno
- Escopo de variáveis

**Uso**: `./compilador testes/teste_procedimentos.pas`

---

### 6. teste_comentarios.pas
**O que testa**: Tratamento de comentários
- Comentários de linha única
- Comentários de múltiplas linhas
- Comentários intercalados com código

**Uso**: `./compilador testes/teste_comentarios.pas`

---

### 7. teste_erro_lexico.pas
**O que testa**: Detecção de erro léxico
- Contém caractere inválido (@)
- Deve parar e reportar erro

**Uso**: `./compilador testes/teste_erro_lexico.pas`

**Saída esperada**:
```
ERRO LÉXICO: caractere inválido '@' (ASCII 64) na linha 5
```

---

### 8. teste_erro_sintatico.pas
**O que testa**: Detecção de erro sintático
- Falta ponto e vírgula após declaração
- Deve parar e reportar erro

**Uso**: `./compilador testes/teste_erro_sintatico.pas`

**Saída esperada**:
```
ERRO SINTÁTICO na linha 3:
  Esperado: ;
  Encontrado: begin
```

---

## ✅ FUNCIONALIDADES IMPLEMENTADAS

### ETAPA 1: Especificações (20%)
- [x] Expressões regulares documentadas no código
- [x] Gramática livre de contexto (seguindo a especificação)
- [x] Vetor de palavras reservadas (em vez de autômatos)
- [x] Lógica de reconhecimento de tokens

### ETAPA 2: Analisador Léxico (40%)
- [x] Função `obter_atomo()` completa
- [x] Reconhecimento de identificadores
- [x] Reconhecimento de números inteiros
- [x] Reconhecimento de palavras reservadas (20 palavras)
- [x] Reconhecimento de operadores:
  - Aritméticos: +, -, *, div
  - Relacionais: <, >, <=, >=, =, <>
  - Lógicos: and, or, not
  - Atribuição: :=
- [x] Reconhecimento de delimitadores: ( ) , ; : .
- [x] Tratamento de comentários /* */
- [x] Tratamento de espaços em branco e quebras de linha
- [x] Detecção de erros léxicos
- [x] Geração de arquivo tokens.txt
- [x] Impressão de tokens na tela
- [x] Struct Token com todos os campos necessários
- [x] Enum TAtomo com todos os tipos de tokens

### ETAPA 3: Analisador Sintático (40%)
- [x] Função `consome()` para validação de tokens
- [x] Análise descendente recursiva
- [x] Funções para todas as regras da gramática:
  - [x] `programa()` - program id ; block .
  - [x] `bloco()` - declarações + comandos
  - [x] `parte_declaracao_variaveis()` - var declarations
  - [x] `declaracao_variavel()` - id : type
  - [x] `tipo()` - integer | boolean
  - [x] `parte_declaracao_subrotinas()` - procedures/functions
  - [x] `declaracao_procedimento()` - procedure declarations
  - [x] `declaracao_funcao()` - function declarations
  - [x] `parametros_formais()` - formal parameters
  - [x] `parte_comandos()` - begin statements end
  - [x] `comando()` - statement dispatch
  - [x] `comando_atribuicao()` - variable := expression
  - [x] `comando_if()` - if then else
  - [x] `comando_while()` - while do
  - [x] `comando_read()` - read()
  - [x] `comando_write()` - write()
  - [x] `comando_composto()` - begin end
  - [x] `expressao()` - expressions with operators
  - [x] `expressao_simples()` - simple expressions
  - [x] `termo()` - terms
  - [x] `fator()` - factors
- [x] Detecção de erros sintáticos com linha e token
- [x] Mensagens de erro descritivas
- [x] Integração completa entre analisadores léxico e sintático

---

## ⚙️ COMO FUNCIONA

### 1. Fluxo de Execução

```
┌─────────────────┐
│ Arquivo .pas    │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ main()          │ ◄─── Abre arquivos
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ obter_atomo()   │ ◄─── ANALISADOR LÉXICO
└────────┬────────┘      Gera tokens
         │
         ▼
┌─────────────────┐
│ programa()      │ ◄─── ANALISADOR SINTÁTICO
│   ↓             │      Verifica gramática
│ consome()       │
└────────┬────────┘
         │
         ▼
┌─────────────────┐
│ tokens.txt      │ ◄─── Arquivo de saída
└─────────────────┘
```

### 2. Analisador Léxico - obter_atomo()

**Passo a passo**:
1. Pula espaços em branco
2. Verifica fim de arquivo → retorna EOS
3. Se encontra `/*` → trata comentário e chama recursivamente
4. Se encontra letra ou `_` → reconhece identificador ou palavra reservada
5. Se encontra dígito → reconhece número
6. Se encontra operador/delimitador → reconhece e retorna token
7. Se encontra caractere inválido → reporta ERRO LÉXICO

**Exemplo de execução**:
```
Entrada: "var a : integer ;"

Tokens gerados:
1# PALAVRA_RESERVADA | var
1# IDENTIFICADOR | a
1# DOIS_PONTOS | :
1# PALAVRA_RESERVADA | integer
1# PONTO_VIRGULA | ;
```

### 3. Analisador Sintático - Análise Descendente

**Função consome()**:
```c
void consome(TAtomo tipo_esperado) {
    if (token_atual.tipo == tipo_esperado) {
        token_atual = obter_atomo();  // Pega próximo token
    } else {
        // ERRO SINTÁTICO
        exit(1);
    }
}
```

**Exemplo - Reconhecendo "program teste ;"**:
```c
void programa() {
    consome(PROGRAM);      // Espera "program"
    consome(IDENTIFICADOR); // Espera identificador
    consome(PONTO_VIRGULA); // Espera ";"
    // ...
}
```

### 4. Gramática → Funções Recursivas

Cada regra da gramática vira uma função:

**Gramática**:
```
<program> ::= program <id> ; <block> .
```

**Código**:
```c
void programa() {
    consome(PROGRAM);
    consome(IDENTIFICADOR);
    consome(PONTO_VIRGULA);
    bloco();
    consome(PONTO);
}
```

---

## 🚨 TRATAMENTO DE ERROS

### Erros Léxicos

**Quando ocorre**:
- Caractere inválido no código fonte
- Comentário não fechado

**Exemplo**:
```pascal
a @ b;  /* @ é inválido */
```

**Saída**:
```
ERRO LÉXICO: caractere inválido '@' (ASCII 64) na linha 3
```

**Ação**: Programa para imediatamente com `exit(1)`

---

### Erros Sintáticos

**Quando ocorre**:
- Token encontrado não corresponde ao esperado
- Sequência de tokens não segue a gramática

**Exemplo**:
```pascal
var a : integer  /* Falta ; */
begin
```

**Saída**:
```
ERRO SINTÁTICO na linha 2:
  Esperado: ;
  Encontrado: begin
```

**Ação**: Programa para imediatamente com `exit(1)`

---

## 📖 EXEMPLOS DE USO

### Exemplo 1: Compilação com Sucesso

**Comando**:
```bash
./compilador testes/teste_basico.pas
```

**Saída no Terminal**:
```
╔════════════════════════════════════════════════════════╗
║        COMPILADOR MINIPASCAL - VERSÃO COMPLETA        ║
║              Análise Léxica e Sintática               ║
╚════════════════════════════════════════════════════════╝

Arquivo fonte: teste_basico.pas
Arquivo de tokens: tokens.txt

=== INICIANDO ANÁLISE LÉXICA ===

1# PALAVRA_RESERVADA | program
1# IDENTIFICADOR | teste
1# PONTO_VIRGULA | ;
2# PALAVRA_RESERVADA | var
2# IDENTIFICADOR | x
2# DOIS_PONTOS | :
2# PALAVRA_RESERVADA | integer
2# PONTO_VIRGULA | ;
3# PALAVRA_RESERVADA | begin
4# IDENTIFICADOR | x
4# ATRIBUICAO | :=
4# NUMERO | 10
4# PONTO_VIRGULA | ;
5# PALAVRA_RESERVADA | end
5# PONTO | .
6# EOF | EOF

=== INICIANDO ANÁLISE SINTÁTICA ===

=== ANÁLISE CONCLUÍDA COM SUCESSO! ===
Programa sintaticamente correto.

╔════════════════════════════════════════════════════════╗
║                  COMPILAÇÃO CONCLUÍDA                  ║
║           Arquivo de tokens gerado com sucesso         ║
╚════════════════════════════════════════════════════════╝
```

**Arquivo tokens.txt**:
```
1# PALAVRA_RESERVADA | program
1# IDENTIFICADOR | teste
1# PONTO_VIRGULA | ;
2# PALAVRA_RESERVADA | var
2# IDENTIFICADOR | x
2# DOIS_PONTOS | :
2# PALAVRA_RESERVADA | integer
2# PONTO_VIRGULA | ;
3# PALAVRA_RESERVADA | begin
4# IDENTIFICADOR | x
4# ATRIBUICAO | :=
4# NUMERO | 10
4# PONTO_VIRGULA | ;
5# PALAVRA_RESERVADA | end
5# PONTO | .
```

---

### Exemplo 2: Erro Léxico

**Comando**:
```bash
./compilador testes/teste_erro_lexico.pas
```

**Saída**:
```
╔════════════════════════════════════════════════════════╗
║        COMPILADOR MINIPASCAL - VERSÃO COMPLETA        ║
║              Análise Léxica e Sintática               ║
╚════════════════════════════════════════════════════════╝

Arquivo fonte: teste_erro_lexico.pas
Arquivo de tokens: tokens.txt

=== INICIANDO ANÁLISE LÉXICA ===

1# PALAVRA_RESERVADA | program
1# IDENTIFICADOR | erro_lexico
1# PONTO_VIRGULA | ;
2# PALAVRA_RESERVADA | var
2# IDENTIFICADOR | a
2# DOIS_PONTOS | :
2# PALAVRA_RESERVADA | integer
2# PONTO_VIRGULA | ;
3# PALAVRA_RESERVADA | begin
4# IDENTIFICADOR | a
4# ATRIBUICAO | :=
4# NUMERO | 10
4# PONTO_VIRGULA | ;
5# IDENTIFICADOR | b

ERRO LÉXICO: caractere inválido '@' (ASCII 64) na linha 5
```

---

### Exemplo 3: Erro Sintático

**Comando**:
```bash
./compilador testes/teste_erro_sintatico.pas
```

**Saída**:
```
╔════════════════════════════════════════════════════════╗
║        COMPILADOR MINIPASCAL - VERSÃO COMPLETA        ║
║              Análise Léxica e Sintática               ║
╚════════════════════════════════════════════════════════╝

Arquivo fonte: teste_erro_sintatico.pas
Arquivo de tokens: tokens.txt

=== INICIANDO ANÁLISE LÉXICA ===

1# PALAVRA_RESERVADA | program
1# IDENTIFICADOR | erro_sintatico
1# PONTO_VIRGULA | ;
2# PALAVRA_RESERVADA | var
2# IDENTIFICADOR | a
2# DOIS_PONTOS | :
2# PALAVRA_RESERVADA | integer
3# PALAVRA_RESERVADA | begin

=== INICIANDO ANÁLISE SINTÁTICA ===

ERRO SINTÁTICO na linha 3:
  Esperado: ;
  Encontrado: begin (begin)
```

---

## 🎨 DECISÕES DE DESIGN

### 1. Uso de Vetor em vez de Autômatos
**Decisão**: Usar array de structs para palavras reservadas  
**Motivo**: Mais simples e acordado com a professora
**Implementação**:
```c
typedef struct {
    const char *palavra;
    TAtomo tipo;
} PalavraReservada;

PalavraReservada palavras_reservadas[] = {
    {"and", AND},
    {"begin", BEGIN},
    // ...
};
```

### 2. Case-Insensitive para Palavras Reservadas
**Decisão**: Converter para minúsculas antes de comparar  
**Motivo**: Facilita reconhecimento (aceita "BEGIN", "begin", "Begin")  
**Implementação**: Função `para_minusculo()`

### 3. Análise Descendente Recursiva
**Decisão**: Usar análise top-down  
**Motivo**: Mais intuitiva e fácil de implementar do que análise ascendente  
**Vantagem**: Cada regra da gramática = uma função

### 4. Parada Imediata em Erros
**Decisão**: Usar `exit(1)` ao encontrar erro  
**Motivo**: Simplifica o código e atende aos requisitos  
**Alternativa não implementada**: Modo de recuperação de erros

### 5. Impressão Dupla (Tela + Arquivo)
**Decisão**: Imprimir tokens tanto na tela quanto em arquivo  
**Motivo**: Facilita debug e atende aos requisitos do projeto

### 6. Expressões entre Parênteses Opcionais
**Decisão**: Aceitar `if (x > 5)` e `if x > 5`  
**Motivo**: Flexibilidade, ambos são válidos em Pascal

---
