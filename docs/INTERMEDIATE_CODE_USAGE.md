# Geração de Código Intermediário - Guia de Uso

## Visão Geral

O **Gerador de Código Intermediário** é um componente crucial do compilador MiniPascal que converte o código-fonte processado em um formato intermediário padronizado (Three-Address Code - TAC), facilitando otimizações e geração de código de máquina.

## Como Funciona

### Fase 1: Compilação
O compilador executa as seguintes fases antes de gerar o código intermediário:
1. **Análise Léxica** - Tokenização do código
2. **Análise Sintática** - Construção da AST
3. **Análise Semântica** - Validação de tipos e escopos
4. **Geração de Código Intermediário** ← Você está aqui!
5. Otimização (próxima fase)
6. Geração de Código (próxima fase)

### Fase 2: Conversão TAC
A AST é percorrida recursivamente, gerando instruções TAC:
- Expressões criam variáveis temporárias (t0, t1, ...)
- Estruturas de controle geram rótulos (L0, L1, ...)
- Cada instrução rastreia a linha do código-fonte

### Fase 3: Saída
Saída dupla:
1. **Terminal** - Tabela formatada em tempo real
2. **Arquivo .tac** - Persistência para processamento posterior

## Uso Básico

### Compilação e Execução

```bash
# Compilar o projeto
cd /workspaces/minipascalCompilerProject
make

# Executar com arquivo de entrada
./bin/compiler caminho/do/arquivo.txt

# Ou sem argumentos (entrada interativa)
./bin/compiler
# Digite o caminho quando solicitado
```

### Saída Esperada

O compilador gera:
1. **Saída no terminal** em 4 fases:
   ```
   ═══════════════════════════════════════════════════════════════════
   MINIPASCAL COMPILER
   ═══════════════════════════════════════════════════════════════════
   
   ───────────────────────────────────────────────────────────────────
   Análise Léxica
   ───────────────────────────────────────────────────────────────────
   [Tabela de símbolos...]
   
   ───────────────────────────────────────────────────────────────────
   Análise Sintática
   ───────────────────────────────────────────────────────────────────
   ✓ análise sintática concluída com sucesso!
   
   ───────────────────────────────────────────────────────────────────
   Análise Semântica
   ───────────────────────────────────────────────────────────────────
   ✓ análise semântica concluída com sucesso!
   
   ───────────────────────────────────────────────────────────────────
   Geração de Código Intermediário
   ───────────────────────────────────────────────────────────────────
   ✓ código intermediário gerado com sucesso!
   
   [Tabela TAC...]
   
   ✓ código intermediário salvo em:
     caminho/do/arquivo.tac
   ```

2. **Arquivo .tac** com:
   - Header informativo
   - Lista de instruções
   - Estatísticas de geração

## Exemplos

### Exemplo 1: Atribuição e Operações

**Entrada (exemplo.txt):**
```pascal
program Exemplo;
var a, b, c : integer;
begin
  a := 5;
  b := 10;
  c := a + b;
end.
```

**Saída TAC (exemplo.tac):**
```
ID  Operação            Resultado    Arg1         Arg2         Linha
000  ASSIGN_LITERAL      a            0            -            2
001  ASSIGN_LITERAL      b            0            -            2
002  ASSIGN_LITERAL      c            0            -            2
003  ASSIGN_LITERAL      a            5            -            5
004  ASSIGN_LITERAL      b            10           -            6
005  ADD                 t0           a            b            7
006  ASSIGN              c            t0           -            7

Total de instruções: 7
Variáveis temporárias criadas: 1
Labels criados: 0
```

### Exemplo 2: Estrutura IF

**Entrada:**
```pascal
program IfExample;
var x : integer;
begin
  x := 10;
  if (x > 5) then
    x := x * 2;
end.
```

**Saída TAC:**
```
ID  Operação            Resultado    Arg1         Arg2         Linha
000  ASSIGN_LITERAL      x            0            -            2
001  ASSIGN_LITERAL      x            10           -            4
002  GT                  t0           x            5            5
003  JUMP_IF_FALSE       -            t0           L0           5
004  MUL                 t1           x            2            6
005  ASSIGN              x            t1           -            6
006  LABEL               L0           -            -            5

Total de instruções: 7
Variáveis temporárias criadas: 2
Labels criados: 1
```

### Exemplo 3: Estrutura WHILE

**Entrada:**
```pascal
program WhileExample;
var x : integer;
begin
  x := 5;
  while (x > 0) do
  begin
    x := x - 1;
  end;
end.
```

**Saída TAC:**
```
ID  Operação            Resultado    Arg1         Arg2         Linha
000  ASSIGN_LITERAL      x            0            -            2
001  ASSIGN_LITERAL      x            5            -            4
002  LABEL               L0           -            -            5
003  GT                  t0           x            0            5
004  JUMP_IF_FALSE       -            t0           L1           5
005  SUB                 t1           x            1            6
006  ASSIGN              x            t1           -            6
007  JUMP                -            L0           -            6
008  LABEL               L1           -            -            5

Total de instruções: 9
Variáveis temporárias criadas: 2
Labels criados: 2
```

## Estrutura dos Arquivos

### Header (intermediateCodeGenerator.h)
Define:
- Tipos de operações TAC
- Estruturas de dados
- Protótipos de funções públicas

### Implementação (intermediateCodeGenerator.c)
Implementa:
- Geração recursiva de TAC
- Tratamento de expressões
- Controle de fluxo
- I/O (terminal e arquivo)
- Gerenciamento de memória

### Integração (main.c)
- Chama o gerador após análise semântica
- Imprime resultado no terminal
- Salva em arquivo .tac
- Libera recursos

## Estruturas de Dados Principais

### TACInstruction
```c
typedef struct {
    TACOpType op;         // Tipo de operação (ADD, SUB, etc)
    char *result;         // Variável/registrador resultado
    char *arg1;           // Primeiro argumento
    char *arg2;           // Segundo argumento
    int line;             // Linha do código-fonte
} TACInstruction;
```

### IntermediateCode
```c
typedef struct {
    TACInstruction *instructions;  // Array de instruções
    int instruction_count;         // Número de instruções
    int instruction_capacity;      // Capacidade alocada
    int temp_counter;              // Contador: t0, t1, ...
    int label_counter;             // Contador: L0, L1, ...
} IntermediateCode;
```

## Operações TAC Suportadas

| Operação | Formato | Exemplo |
|----------|---------|---------|
| Atribuição | `result = arg1` | `x = a` |
| Literal | `result = literal` | `x = 10` |
| Adição | `result = arg1 + arg2` | `t0 = a + b` |
| Subtração | `result = arg1 - arg2` | `t1 = x - 1` |
| Multiplicação | `result = arg1 * arg2` | `t2 = y * 2` |
| Divisão | `result = arg1 / arg2` | `t3 = a / b` |
| Módulo | `result = arg1 mod arg2` | `t4 = a mod b` |
| Comparação | `result = arg1 op arg2` | `t5 = x < 10` |
| Lógico AND | `result = arg1 && arg2` | `t6 = p && q` |
| Lógico OR | `result = arg1 \|\| arg2` | `t7 = p \|\| q` |
| Negação | `result = !arg1` | `t8 = !p` |
| Rótulo | `label:` | `L0:` |
| Jump | `goto label` | `goto L1` |
| Jump Condicional | `if !arg1 goto label` | `if !t0 goto L2` |
| Entrada | `read var` | `read x` |
| Saída | `write expr` | `write x` |
| Chamada Função | `call func(args)` | `call func()` |
| Retorno | `return arg` | `return x` |

## Arquivos Gerados

### Arquivo .tac
```
; ============================================
; MiniPascal - Código Intermediário Gerado
; ============================================

; ID  Operação            Resultado    Arg1         Arg2         Linha
; ----+------------------+------------+------------+----------+---------
000  ASSIGN_LITERAL      x            0            -            2
...

; ============================================
; Estatísticas
; ============================================
; Total de instruções: 8
; Variáveis temporárias criadas: 1
; Labels criados: 0
```

## Teste

Um script de teste está disponível:
```bash
chmod +x /workspaces/minipascalCompilerProject/test_intermediate_code.sh
./test_intermediate_code.sh
```

Este script:
1. Compila o projeto
2. Executa o compilador com arquivo de teste
3. Verifica a geração do arquivo .tac
4. Exibe os resultados

## Troubleshooting

### Nenhuma instrução gerada
**Problema**: Arquivo .tac contém "(nenhuma instrução gerada)"
**Solução**: Verifique se o código-fonte foi processado corretamente pelas análises anteriores.

### Arquivo .tac não criado
**Problema**: Arquivo .tac não aparece após compilação
**Solução**: Verifique permissões de escrita no diretório de saída.

### Erro na compilação
**Problema**: Erro "undefined reference" ao compilar
**Solução**: Certifique-se de que `intermediateCodeGenerator.c` está no Makefile.

## Próximas Fases

Com o código intermediário TAC gerado, as próximas fases são:
1. **Otimização de Código Intermediário** - Elimina código morto, constantes, etc.
2. **Geração de Código de Máquina** - Converte TAC em assembly ou bytecode
3. **Otimização Dependente de Máquina** - Otimizações específicas do processador

## Referências

- **TAC (Three-Address Code)**: Formato intermediário padrão em compiladores
- **AST (Abstract Syntax Tree)**: Estrutura gerada pela análise sintática
- **Engenharia de Compiladores**: Aho, Lam, Sethi, Ullman (Dragon Book)

---

**Última atualização**: Novembro 2025
**Versão**: 1.0
**Status**: Implementado e Funcional
