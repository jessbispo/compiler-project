# MiniPascal Compiler Project

Este projeto consiste na implementação de um compilador para a linguagem **MiniPascal**, seguindo as etapas clássicas do processo de compilação: análise léxica, análise sintática, análise semântica e geração de código intermediário. A estrutura foi organizada para manter clareza, modularidade e facilitar futuras expansões para otimizações e geração de código final.

O compilador foi desenvolvido inteiramente em **C**, utilizando o compilador **MinGW (GCC)**, conforme solicitado pela disciplina. Todas as etapas implementadas seguem a especificação dos PDFs fornecidos no diretório `docs/project-specification`.

---

# 1. Objetivo do Projeto

O objetivo principal é construir um compilador funcional para um subconjunto da linguagem Pascal (MiniPascal), que:

- lê um arquivo de entrada contendo um programa MiniPascal;  
- realiza análise léxica, sintática e semântica;  
- gera a tabela de símbolos;  
- produz a árvore sintática abstrata (AST);  
- gera código intermediário em formato de três endereços;  
- registra erros durante o processo de compilação;  
- salva arquivos de saída organizados em `out/`.

O compilador final deve obedecer aos critérios exigidos na disciplina, principalmente:

- ausência de *warnings* ao compilar;  
- término com código de retorno igual a 0;  
- clareza do código e padronização na indentação;  
- fidelidade ao formato do arquivo de entrada;  
- entrega acompanhada de um arquivo README;  
- apresentação do projeto.

---

# 2. Estrutura do Projeto

A organização foi feita de forma modular, separando claramente o frontend do compilador dos utilitários internos.

```
| estrutura.txt
| LICENSE
| Makefile
| README.md
|
+---bin
| compiler
|
+---build
| | main.o
| |
| +---frontend
| | intermediateCodeGenerator.o
| | lexicalAnalysis.o
| | semanticAnalysis.o
| | syntacticAnalysis.o
| |
| ---utility
| +---afn
| +---beautifulPrint
| +---errorHandler
| +---fileHandler
| ---symbolTable
|
+---docs
| +---afn
| ---project-specification
|
+---include
| *.h
|
+---out
| ast.txt
| intermediate_code.txt
| tokens.txt
|
+---samples
| ---valid
|
+---src
| | main.c
| |
| +---frontend
| | intermediateCodeGenerator.c
| | lexicalAnalysis.c
| | semanticAnalysis.c
| | syntacticAnalysis.c
| |
| ---utility
| (AFN, tabela de símbolos, tratamento de erros, etc.)
|
---testes
saida_testes.txt

```


### Principais diretórios

#### `src/frontend`
Contém cada módulo da pipeline:
- análise léxica  
- análise sintática  
- análise semântica  
- geração de código intermediário  

#### `src/utility`
Ferramentas internas:
- autômatos finitos (AFN) para reconhecimento de tokens  
- tabela de símbolos  
- manipulador de arquivos  
- tratamento de erros  
- impressões formatadas  

#### `out/`
Arquivos gerados automaticamente:
- `tokens.txt`  
- `ast.txt`  
- `intermediate_code.txt`  

---

# 3. Etapas Implementadas

## 3.1 Análise Léxica
Responsável por:
- leitura do arquivo de entrada  
- identificação e classificação de tokens  
- remoção de comentários  
- detecção de erros léxicos  
- geração de `out/tokens.txt`  

A identificação é feita utilizando autômatos presentes em `utility/afn`.

---

## 3.2 Análise Sintática
Implementada por um analisador descendente baseado na gramática do MiniPascal.

Funções principais:
- valida a estrutura do programa  
- constrói a AST  
- pausa o processo em caso de erros críticos  

A AST é salva em `out/ast.txt`.

---

## 3.3 Análise Semântica
Valida:
- declarações  
- compatibilidade de tipos  
- escopos  
- chamadas de procedimentos  
- variáveis não declaradas  

A tabela de símbolos é atualizada dinamicamente ao longo dessa etapa.

---

## 3.4 Geração de Código Intermediário
O código é produzido no formato de **três endereços**, incluindo:

- operações aritméticas  
- comparações  
- saltos condicionais  
- saltos incondicionais  
- labels  
- atribuições  
- escrita de valores (`WRITE`)  
- temporários (`t0`, `t1`, ...)  

Resultado salvo em:

    out/intermediate_code.txt

    
---

# 4. Como Compilar

Você pode usar diretamente o GCC, mas por ser um processo que exige compilar mais de um arquivo .c, indicamos utilizar o Makefile com o comando:

    make


O Makefile cuida de:
- compilar sem warnings  
- organizar os `.o` em `build/`  
- gerar o binário final em `bin/`  

---

# 5. Como Executar

Após compilar, estando na raiz do projeto:

    ./bin/compiler samples/valid/minipascal-code-accepted.txt

