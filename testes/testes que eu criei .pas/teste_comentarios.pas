/* 
 * Teste de comentários
 * Este programa testa todas as formas de comentários
 * em MiniPascal
 */

program comentarios; /* comentário na mesma linha da declaração */

/* Declaração de variáveis com comentários */
var a, b, c : integer; /* três variáveis inteiras */
var x, y : boolean; /* duas variáveis booleanas */
var resultado : integer; /* variável para armazenar resultados */

/* 
 * Procedure de exemplo
 * com comentário de múltiplas linhas
 */
procedure testar(var n : integer);
var temp : integer; /* variável temporária */
begin
    /* Incrementa o valor */
    temp := 1;
    n := n + temp; /* adiciona temp a n */
end; /* fim da procedure */

/* Início do programa principal */
begin
    /* 
     * Seção 1: Inicialização
     * Aqui inicializamos as variáveis
     */
    a := 10; /* atribui 10 a 'a' */
    b := 20; /* atribui 20 a 'b' */
    c := 0; /* inicializa 'c' com zero */
    
    /* Seção 2: Operações aritméticas */
    resultado := a + b; /* soma */
    resultado := a - b; /* subtração */
    resultado := a * b; /* multiplicação */
    resultado := a div b; /* divisão inteira */
    
    /* Comentário com caracteres especiais: @#$%^&*()_+-={}[]|:;<>,.?/ */
    
    /*
    Este é um comentário grande
    que ocupa várias linhas
    e pode conter código comentado:
    
    a := 999;
    b := 888;
    write(a, b);
    
    Tudo isso será ignorado pelo compilador
    */
    
    /* Seção 3: Estruturas de controle */
    if a > b then /* testa se a é maior que b */
        resultado := a /* atribui a ao resultado */
    else
        resultado := b; /* caso contrário, atribui b */
    
    /* Loop para contar até 10 */
    c := 0;
    while c < 10 do /* enquanto c menor que 10 */
    begin
        c := c + 1; /* incrementa c */
        /* write(c); */ /* esta linha está comentada */
    end; /* fim do while */
    
    /* Seção 4: Variáveis booleanas */
    x := true; /* atribui verdadeiro */
    y := false; /* atribui falso */
    
    /* Operações lógicas */
    x := x and y; /* E lógico */
    x := x or y; /* OU lógico */
    x := not x; /* NÃO lógico */
    
    /* Seção 5: Testes com comentários intercalados */
    a /* comentário no meio */ := /* outro comentário */ 5;
    
    /* Chamada de procedure com comentário */
    testar(a); /* incrementa 'a' */
    
    /* Comando write com comentário */
    write(a); /* imprime o valor de 'a' */
    write(b, c); /* imprime b e c */
    
    /* 
     * Fim do programa
     * Todos os comentários foram testados com sucesso
     */
end. /* ponto final */

/* Comentário após o fim do programa (será ignorado) */