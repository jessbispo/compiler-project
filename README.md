# Projeto Compilador MiniPascal - Fase 1

## Informações do Projeto
**Disciplina:** Compiladores  
**Fase:** 1 - Análise Léxica e Análise Sintática  
**Data de Entrega:** 30/09  
**Integrantes:** Jessica, Vitor

## Descrição do Projeto

Este projeto implementa as duas primeiras etapas do front-end de um compilador para a linguagem MiniPascal, uma simplificação da linguagem Pascal que suporta apenas tipos inteiros e booleanos.

### Características da Linguagem MiniPascal:
- Case sensitive
- Suporte apenas para tipos `integer` e `boolean`
- Subconjunto válido da linguagem Pascal
- Comentários delimitados por `/* */`
- Operadores: `+`, `-`, `div`, `*`, `and`, `or`, `not`, `<`, `>`, `<>`, `<=`, `>=`, `:=`
- Palavras reservadas: `program`, `begin`, `end`, `procedure`, `if`, `then`, `else`, `while`, `do`, `var`, `integer`, `boolean`, `true`, `false`, `write`, `read`

## Estrutura do Projeto

O projeto está dividido em 3 etapas:

### Etapa 1 (20%) - Gramática e Autômatos
- [x] Definição de expressões regulares
- [x] Construção da gramática livre de contexto
- [x] Autômatos finitos para cada classe de token
- [x] Autômato unificado para todos os elementos

### Etapa 2 (40%) - Analisador Léxico
- [ ] Implementação do analisador léxico em C
- [ ] Função `obter_atomo()` para geração de tokens
- [ ] Tratamento de erros léxicos
- [ ] Saída formatada com linha e tipo de token
- [ ] Estrutura de dados para tokens (enum TAtomo)

### Etapa 3 (40%) - Analisador Sintático
- [ ] Implementação do analisador sintático
- [ ] Função `consome()` para interação com analisador léxico
- [ ] Análise baseada na gramática da linguagem
- [ ] Tratamento de erros sintáticos
- [ ] Integração completa dos analisadores

## Como Compilar e Executar

### Compilação:
```bash
gcc -Wall -Wno-unused-result -g -Og compilador.c -o compilador
```

### Execução:
```bash
./compilador nome_do_arquivo.pas
```

## Formato de Saída

O analisador léxico produz saída no formato:
```
Número da Linha # NomeToken | Atributo
```

Exemplo:
```
11# IDENTIFICADOR | var1
12# NUMERO | 42
13# OPERADOR | :=
```

## Tratamento de Erros

### Erros Léxicos:
- Mensagem: "ERRO LÉXICO"
- Exibe a sequência lexicamente incorreta
- Finaliza o processo

### Erros Sintáticos:
- Mensagem: "ERRO SINTÁTICO"
- Exibe o token incorreto e linha correspondente
- Finaliza o processo

## Arquivos do Projeto

- `compilador.c` - Código principal do compilador
- `README.txt` - Este arquivo de documentação
- `automatos.pdf` - Diagramas dos autômatos (Etapa 1)
- `gramatica.txt` - Gramática livre de contexto definida
- `exemplos/` - Diretório com programas MiniPascal de teste

## Exemplo de Código MiniPascal

```pascal
program exemplo ;
var a, b : integer ;
var flag : boolean ;
begin
    a := 10 ;
    b := 20 ;
    flag := true ;
    if ( a < b ) then
        write ( a ) ;
    while ( a > 0 ) do
    begin
        a := a - 1 ;
    end ;
end .
```

## Status da Implementação

- Etapa 1: Gramática e autômatos finitos
- Etapa 2: Analisador léxico
- Etapa 3: Analisador sintático

## Decisões de Design

### Estrutura de Dados:
- Utilizamos enum para definir tipos de átomos
- Struct para armazenar informações completas dos tokens
- Buffer circular para leitura eficiente do arquivo fonte

### Tratamento de Comentários:
- Implementado como estado especial no autômato
- Ignorados durante a análise léxica
- Suporte a comentários aninhados (decisão de implementação)

## Bugs Conhecidos

- [Listar aqui qualquer bug identificado durante o desenvolvimento]
- [Exemplo: "Problema com reconhecimento de números negativos em expressões"]

## Limitações Atuais

- [Listar limitações da implementação atual]
- [Exemplo: "Não suporta ainda análise de procedures com parâmetros"]

## Testes Realizados

### Casos de Teste Válidos:
- Programa básico com declarações de variáveis
- Estruturas condicionais (if-then-else)
- Estruturas de repetição (while-do)
- Expressões aritméticas e booleanas

### Casos de Teste Inválidos:
- Identificadores iniciando com números
- Caracteres não reconhecidos
- Estruturas sintáticas incorretas

## Ambiente de Desenvolvimento

- **Compilador:** MinGW com VSCode
- **Linguagem:** C (padrão C99)
- **SO de Desenvolvimento:** [Windows/Linux/macOS]
- **Ferramenta para Autômatos:** JFLAP

