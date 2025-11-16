# Arquitetura do Gerador de Código Intermediário

## 📊 Diagrama de Componentes

```
┌─────────────────────────────────────────────────────────────────────────┐
│                     MINIPASCAL COMPILER ARCHITECTURE                    │
└─────────────────────────────────────────────────────────────────────────┘

                              CÓDIGO-FONTE
                                  │
                                  ▼
                     ┌──────────────────────┐
                     │ Análise Léxica       │
                     │ (tokenização)        │
                     └──────────────────────┘
                                  │
                          Tabela de Símbolos
                          Tabela de Erros
                                  │
                                  ▼
                          Stream de Tokens
                                  │
                                  ▼
                     ┌──────────────────────┐
                     │ Análise Sintática    │
                     │ (construção AST)     │
                     └──────────────────────┘
                                  │
                                  ▼
                        AST (Árvore Sintática)
                                  │
                                  ▼
                     ┌──────────────────────┐
                     │ Análise Semântica    │
                     │ (validação)          │
                     └──────────────────────┘
                                  │
                          Tabela de Símbolos
                          (atualizada)
                                  │
                                  ▼
                     ┌──────────────────────────────┐
                     │ GERAÇÃO CÓDIGO INTERMEDIÁRIO  │ ◄─── IMPLEMENTADO
                     │ (TAC Generation)             │
                     └──────────────────────────────┘
                                  │
                    ┌─────────────┬─────────────┐
                    │             │             │
                    ▼             ▼             ▼
              Terminal        Arquivo .tac   Próximas Fases
           (tabela TAC)   (persistência)   (otimização)
```

## 🔄 Fluxo de Processamento TAC

```
AST (Entrada)
    │
    ▼
createIntermediateCode()
    │ Aloca estrutura IntermediateCode
    │
    ▼
processASTNode(root)
    │
    ├─ Se PROGRAM: processa todos filhos
    │
    ├─ Se VAR_DECL: gera ASSIGN_LITERAL 0
    │
    ├─ Se PROCEDURE: marca FUNCTION_START/END
    │
    ├─ Se STATEMENT:
    │   ├─ ASSIGN: expresão → resultado
    │   ├─ IF: cria labels, JUMP_IF_FALSE
    │   ├─ WHILE: cria labels, JUMP, loops
    │   ├─ BEGIN: processa sequência
    │   ├─ WRITE: gera TAC_WRITE
    │   ├─ READ: gera TAC_READ
    │   └─ CALL: gera PARAM, CALL
    │
    └─ Recursivamente para filhos
        │
        ▼
    generateCodeForExpression()
        │
        ├─ Se literais/identificadores: retorna nome
        │
        ├─ Se expressão binária:
        │   ├─ Gera código subexpressões
        │   ├─ Cria temporário (t0, t1, ...)
        │   ├─ Adiciona instrução TAC apropriada
        │   └─ Retorna temporário
        │
        └─ Se expressão unária:
            ├─ Gera código subexpressão
            ├─ Cria temporário
            ├─ Adiciona instrução TAC
            └─ Retorna temporário
                    │
                    ▼
              addTACInstruction()
                    │
                    ├─ Expande array se necessário
                    ├─ Duplica strings de argumentos
                    ├─ Armazena em estrutura
                    └─ Incrementa contador

IntermediateCode (Saída)
    │
    ├─ instructions[] - Array de TACInstruction
    ├─ instruction_count - Número de instruções
    ├─ temp_counter - t0, t1, ... gerados
    └─ label_counter - L0, L1, ... gerados
        │
        ├─→ printIntermediateCode() → Terminal
        │
        └─→ writeIntermediateCodeToFile() → Arquivo .tac
```

## 📦 Estrutura de Dados

```
┌─────────────────────────────────────────┐
│        IntermediateCode                 │
├─────────────────────────────────────────┤
│ • TACInstruction *instructions          │
│   (array dinâmico)                      │
│                                         │
│ • int instruction_count                 │
│   (número de instruções)                │
│                                         │
│ • int instruction_capacity              │
│   (capacidade alocada)                  │
│                                         │
│ • int temp_counter → t0, t1, t2...      │
│                                         │
│ • int label_counter → L0, L1, L2...     │
└─────────────────────────────────────────┘
          │
          │ Array de
          ▼
┌─────────────────────────────────────────┐
│        TACInstruction                   │
├─────────────────────────────────────────┤
│ • TACOpType op                          │
│   (28 tipos de operações)               │
│                                         │
│ • char *result                          │
│   (variável resultado)                  │
│                                         │
│ • char *arg1                            │
│   (primeiro argumento)                  │
│                                         │
│ • char *arg2                            │
│   (segundo argumento)                   │
│                                         │
│ • int line                              │
│   (linha do código-fonte)               │
└─────────────────────────────────────────┘
```

## 🔗 Relacionamento de Módulos

```
┌──────────────────┐
│   main.c         │
│                  │
│ Coordena as fases│◄──────────────────┐
│ do compilador    │                   │
└────────┬─────────┘                   │
         │                            │
         ▼                            │
┌──────────────────────────────────────────────────┐
│                                                  │
│   ANÁLISE SEMÂNTICA (semanticAnalysis.c)        │
│   ┌─────────────────────────────────────┐       │
│   │ Valida tipos e escopos              │       │
│   │ Retorna AST validada                │       │
│   └─────────────────────────────────────┘       │
│                                                  │
└────────┬─────────────────────────────────────────┘
         │
         ▼
┌──────────────────────────────────────────────────┐
│                                                  │
│   GERAÇÃO CÓDIGO INTERMEDIÁRIO ◄─── NOVO        │
│   ┌─────────────────────────────────────┐       │
│   │ intermediateCodeGenerator.c         │       │
│   │ ─────────────────────────────────── │       │
│   │ • generateIntermediateCode()        │       │
│   │ • generateCodeForStatement()        │       │
│   │ • generateCodeForExpression()       │       │
│   │ • addTACInstruction()               │       │
│   │ • printIntermediateCode()           │       │
│   │ • writeIntermediateCodeToFile()     │       │
│   │ • freeIntermediateCode()            │       │
│   │ • tacOpToString()                   │       │
│   │ • Funções auxiliares                │       │
│   └─────────────────────────────────────┘       │
│   Entrada: AST + Symbol Table                   │
│   Saída: IntermediateCode (TAC)                 │
│   ├─ Terminal (tabela formatada)                │
│   └─ Arquivo .tac                              │
│                                                  │
└──────────────────────────────────────────────────┘
         │
         ▼
    [PRÓXIMAS FASES]
    (Otimização, Geração de Código)
         │
         └────────────────────────────┘
```

## 🎯 Mapeamento AST → TAC

```
AST Node               → TAC Instructions
─────────────────────────────────────────────────────────────

IDENTIFIER            → [nome da variável]

NUMBER                → [valor numérico]

PLUS (a + b)          → t0 = a + b          (TAC_ADD)

MINUS (a - b)         → t1 = a - b          (TAC_SUB)

MUL (a * b)           → t2 = a * b          (TAC_MUL)

DIV (a / b)           → t3 = a / b          (TAC_DIV)

MOD (a mod b)         → t4 = a mod b        (TAC_MOD)

LT (a < b)            → t5 = a < b          (TAC_LT)

GT (a > b)            → t6 = a > b          (TAC_GT)

LE (a <= b)           → t7 = a <= b         (TAC_LE)

GE (a >= b)           → t8 = a >= b         (TAC_GE)

EQ (a == b)           → t9 = a == b         (TAC_EQ)

NE (a != b)           → t10 = a != b        (TAC_NE)

AND (a && b)          → t11 = a && b        (TAC_AND)

OR (a || b)           → t12 = a || b        (TAC_OR)

NOT (!a)              → t13 = !a            (TAC_NOT)

ASSIGN (x := expr)    → x = t0              (TAC_ASSIGN)

VAR_DECL (var x)      → x = 0               (TAC_ASSIGN_LITERAL)

IF-THEN               → if !cond goto L0    (TAC_JUMP_IF_FALSE)
                        [corpo]
                        L0:

IF-THEN-ELSE          → if !cond goto L0    (TAC_JUMP_IF_FALSE)
                        [corpo then]
                        goto L1              (TAC_JUMP)
                        L0:
                        [corpo else]
                        L1:

WHILE                 → L0:                 (TAC_LABEL)
                        if !cond goto L1    (TAC_JUMP_IF_FALSE)
                        [corpo]
                        goto L0              (TAC_JUMP)
                        L1:

BEGIN-END             → [instruções sequência]

WRITE (expr)          → write t0            (TAC_WRITE)

READ (var)            → read x              (TAC_READ)

CALL (func(args))     → param arg1          (TAC_PARAM)
                        param arg2
                        call func            (TAC_CALL)

RETURN (expr)         → return t0           (TAC_RETURN)

PROCEDURE (def)       → func:               (TAC_FUNCTION_START)
                        [corpo]
                        end func             (TAC_FUNCTION_END)
```

## 📈 Estatísticas de Processamento

```
Exemplo: programa.txt
├─ Código-fonte: 15 linhas
├─ Análise Léxica:
│  ├─ Tokens: 45
│  └─ Símbolos tabelados: 8
├─ Análise Sintática:
│  ├─ AST nodes: 42
│  └─ Altura da árvore: 7
├─ Análise Semântica:
│  ├─ Verificações: 23
│  └─ Erros: 0
└─ Geração TAC:
   ├─ Instruções geradas: 13
   ├─ Variáveis temporárias: 3 (t0, t1, t2)
   ├─ Rótulos criados: 2 (L0, L1)
   ├─ Saída terminal: tabela de 13 linhas
   └─ Arquivo gerado: programa.tac (520 bytes)
```

## 🔀 Fluxo de Decisão na Geração

```
Para cada nó da AST:

├─ É PROGRAM?
│  └─→ Processa recursivamente todos os filhos
│
├─ É VAR_DECL?
│  └─→ Gera ASSIGN_LITERAL (inicializa com 0)
│
├─ É PROCEDURE?
│  ├─→ Gera FUNCTION_START
│  ├─→ Processa corpo
│  └─→ Gera FUNCTION_END
│
├─ É STATEMENT?
│  │
│  ├─ É ASSIGN?
│  │  ├─→ Gera código para expressão → temp
│  │  └─→ Gera ASSIGN (variável = temp)
│  │
│  ├─ É IF?
│  │  ├─→ Gera código para condição
│  │  ├─→ Gera JUMP_IF_FALSE
│  │  ├─→ Processa então-ramo
│  │  ├─→ Gera JUMP (pula else)
│  │  ├─→ Label false
│  │  ├─→ Processa else-ramo (se houver)
│  │  └─→ Label end
│  │
│  ├─ É WHILE?
│  │  ├─→ Label start
│  │  ├─→ Gera código para condição
│  │  ├─→ Gera JUMP_IF_FALSE
│  │  ├─→ Processa corpo
│  │  ├─→ Gera JUMP (volta ao start)
│  │  └─→ Label false (fim do loop)
│  │
│  ├─ É BEGIN?
│  │  └─→ Processa sequência de statements
│  │
│  ├─ É WRITE?
│  │  ├─→ Gera código para expressão → temp
│  │  └─→ Gera WRITE (temp)
│  │
│  ├─ É READ?
│  │  └─→ Gera READ (variável)
│  │
│  └─ É CALL?
│     ├─→ Para cada argumento:
│     │  ├─→ Gera código para argumento → temp
│     │  └─→ Gera PARAM (temp)
│     └─→ Gera CALL (função)
│
└─ Outro tipo?
   └─→ Processa recursivamente (travessia genérica)
```

## 💾 Modelo de Memória

```
Alocação Dinâmica:

IntermediateCode
├─ instructions: malloc(capacity * sizeof(TACInstruction))
│  └─ Realloc quando instruction_count >= capacity
│     (capacity *= 2)
│
├─ Para cada TACInstruction:
│  ├─ result: malloc(strlen + 1) se não NULL
│  ├─ arg1: malloc(strlen + 1) se não NULL
│  └─ arg2: malloc(strlen + 1) se não NULL

Cleanup:
└─ freeIntermediateCode()
   ├─ Para cada instrução:
   │  ├─ free(result)
   │  ├─ free(arg1)
   │  └─ free(arg2)
   ├─ free(instructions)
   └─ free(code)
```

---

**Diagrama Arquitetural Completo do Gerador de Código Intermediário**
*Novembro 2025*
