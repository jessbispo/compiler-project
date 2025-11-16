# Geração de Código Intermediário - MiniPascal Compiler

## Descrição

A geração de código intermediário (Intermediate Code Generation) é a fase do compilador que converte a Árvore Sintática Abstrata (AST) gerada pela análise sintática em um código intermediário padronizado.

Nesta implementação, foi utilizado o modelo **Three-Address Code (TAC)** - Código de Três Endereços, um formato intermediário amplamente usado em compiladores modernos.

## Estrutura da Implementação

### 1. Formato TAC (Three-Address Code)

Cada instrução TAC possui no máximo 3 operandos:
```
resultado = operando1 operador operando2
```

Exemplos:
```
t0 = a + b        ; Adição
t1 = t0 * c       ; Multiplicação
if !cond goto L1  ; Desvio condicional
L1: x = 10        ; Label
```

### 2. Tipos de Instruções Suportadas

A implementação suporta os seguintes tipos de operações TAC:

#### Operações Aritméticas
- `TAC_ADD` - Adição: `result = arg1 + arg2`
- `TAC_SUB` - Subtração: `result = arg1 - arg2`
- `TAC_MUL` - Multiplicação: `result = arg1 * arg2`
- `TAC_DIV` - Divisão: `result = arg1 / arg2`
- `TAC_MOD` - Módulo: `result = arg1 mod arg2`

#### Operações Lógicas
- `TAC_LT` - Menor que: `result = arg1 < arg2`
- `TAC_GT` - Maior que: `result = arg1 > arg2`
- `TAC_LE` - Menor ou igual: `result = arg1 <= arg2`
- `TAC_GE` - Maior ou igual: `result = arg1 >= arg2`
- `TAC_EQ` - Igual: `result = arg1 == arg2`
- `TAC_NE` - Não igual: `result = arg1 != arg2`
- `TAC_AND` - E lógico: `result = arg1 && arg2`
- `TAC_OR` - OU lógico: `result = arg1 || arg2`
- `TAC_NOT` - Negação: `result = !arg1`

#### Operações de Controle de Fluxo
- `TAC_LABEL` - Rótulo: `label:`
- `TAC_JUMP` - Desvio incondicional: `goto label`
- `TAC_JUMP_IF_FALSE` - Desvio condicional (falso): `if !cond goto label`
- `TAC_JUMP_IF_TRUE` - Desvio condicional (verdadeiro): `if cond goto label`

#### Operações de Atribuição
- `TAC_ASSIGN` - Atribuição: `result = arg1`
- `TAC_ASSIGN_LITERAL` - Atribuição de literal: `result = constante`

#### Operações de Função
- `TAC_FUNCTION_START` - Início de função: `function name:`
- `TAC_FUNCTION_END` - Fim de função: `end function`
- `TAC_CALL` - Chamada de função: `call function(args)`
- `TAC_PARAM` - Parâmetro: `param arg1`
- `TAC_RETURN` - Retorno: `return arg1`

#### Operações de I/O
- `TAC_READ` - Entrada: `read var`
- `TAC_WRITE` - Saída: `write expr`

#### Operações de Array
- `TAC_ARRAY_ACCESS` - Acesso a array: `result = array[index]`
- `TAC_ARRAY_ASSIGN` - Atribuição a array: `array[index] = value`

### 3. Estruturas de Dados

#### IntermediateCode
```c
typedef struct {
    TACInstruction *instructions;  // Array de instruções
    int instruction_count;         // Número de instruções geradas
    int instruction_capacity;      // Capacidade de armazenamento
    int temp_counter;              // Contador de variáveis temporárias
    int label_counter;             // Contador de rótulos
} IntermediateCode;
```

#### TACInstruction
```c
typedef struct {
    TACOpType op;      // Tipo de operação
    char *result;      // Variável resultado
    char *arg1;        // Primeiro argumento
    char *arg2;        // Segundo argumento
    int line;          // Linha da instrução no código-fonte
} TACInstruction;
```

### 4. Funções Principais

#### Geração
- `createIntermediateCode()` - Cria e inicializa a estrutura
- `generateIntermediateCode(AST, symtab, errtab)` - Gera TAC a partir da AST
- `addTACInstruction()` - Adiciona uma instrução ao código

#### Saída
- `printIntermediateCode()` - Imprime o código intermediário no terminal em formato tabular
- `writeIntermediateCodeToFile()` - Escreve o código em arquivo (.tac)

#### Utilitários
- `tacOpToString()` - Converte tipo de operação para string legível
- `freeIntermediateCode()` - Libera memória alocada

## Processamento da AST

O processo de geração percorre a AST recursivamente, tratando:

1. **Declarações de Variáveis** - Inicializam variáveis com 0
2. **Atribuições** - Gera código para calcular expressão e atribuir
3. **Expressões** - Cria variáveis temporárias (t0, t1, ...) para resultados
4. **Estruturas de Controle**:
   - `IF` - Gera rótulos e desvios condicionais
   - `WHILE` - Gera loop com rótulo de início e fim
   - `BEGIN...END` - Processa sequência de comandos
5. **I/O** - `READ` e `WRITE` geram instruções correspondentes
6. **Funções** - Marca início/fim e processa corpo

## Exemplos de Saída

### Código MiniPascal
```pascal
program Exemplo;
var a, b, c : integer;
begin
  a := 5;
  b := 10;
  c := a + b;
  write(c);
end.
```

### Código Intermediário Gerado
```
ID  Operação            Resultado    Arg1         Arg2         Linha
000  ASSIGN_LITERAL      a            0            -            2
001  ASSIGN_LITERAL      b            0            -            2
002  ASSIGN_LITERAL      c            0            -            2
003  ASSIGN_LITERAL      a            5            -            5
004  ASSIGN_LITERAL      b            10           -            6
005  ADD                 t0           a            b            7
006  ASSIGN              c            t0           -            7
007  WRITE               -            c            -            8

Total de instruções: 8
Variáveis temporárias criadas: 1
Labels criados: 0
```

## Integração com o Compilador

O gerador de código intermediário é acionado após as análises:
1. Análise Léxica ✓
2. Análise Sintática ✓
3. Análise Semântica ✓
4. **Geração de Código Intermediário** ← Aqui
5. Otimização (próxima fase)
6. Geração de Código de Máquina (próxima fase)

### Fluxo no main.c
```c
// Após análise semântica bem-sucedida
IntermediateCode *intermediateCode = generateIntermediateCode(ast, symtab, errtab);

// Imprime no terminal
printIntermediateCode(intermediateCode);

// Salva em arquivo .tac
writeIntermediateCodeToFile(intermediateCode, "output.tac");

// Libera memória
freeIntermediateCode(intermediateCode);
```

## Arquivos Gerados

O compilador gera:
1. **Saída no Terminal** - Tabela formatada com todas as instruções TAC
2. **Arquivo .tac** - Arquivo texto com as mesmas instruções para posterior processamento

Exemplo de arquivo gerado (`programa.tac`):
```
; ============================================
; MiniPascal - Código Intermediário Gerado
; ============================================

; ID  Operação            Resultado    Arg1         Arg2         Linha
; ----+------------------+------------+------------+----------+---------
000  ASSIGN_LITERAL      x            0            -            2
001  ASSIGN_LITERAL      y            0            -            2
...

; ============================================
; Estatísticas
; ============================================
; Total de instruções: 8
; Variáveis temporárias criadas: 1
; Labels criados: 0
```

## Características da Implementação

✓ **TAC Padronizado** - Segue convenções de compiladores clássicos
✓ **Variáveis Temporárias** - Nomeadas como t0, t1, t2...
✓ **Rótulos** - Nomeados como L0, L1, L2...
✓ **Rastreamento de Linha** - Cada instrução mantém linha do código-fonte
✓ **Gerenciamento de Memória** - Alocação dinâmica e liberação adequada
✓ **Saída Formatada** - Tabelas legíveis em terminal e arquivo
✓ **Expansão Automática** - Dinâmica do vetor de instruções conforme necessário

## Próximos Passos

Esta implementação de código intermediário TAC oferece base sólida para:
1. Otimizações de código intermediário
2. Geração de código de máquina
3. Análise de fluxo de dados
4. Detecção de código morto
5. Alocação de registradores

---

**Versão**: 1.0
**Data**: Novembro 2025
**Linguagem**: C (padrão C99)
