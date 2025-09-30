/* Teste de procedures e functions */
program procedimentos;
var global1, global2, global3 : integer;
var resultado : integer;

/* Procedure simples sem parâmetros */
procedure inicializar;
var temp : integer;
begin
    temp := 0;
    global1 := temp;
    global2 := temp;
end;

/* Procedure com um parâmetro */
procedure dobrar(var x : integer);
begin
    x := x * 2;
end;

/* Procedure com múltiplas variáveis locais */
procedure calcular(var res : integer);
var a, b, c : integer;
begin
    a := 5;
    b := 10;
    c := 15;
    res := a + b + c;
end;

/* Procedure com parâmetro var */
procedure incrementar(var numero : integer);
var inc : integer;
begin
    inc := 1;
    numero := numero + inc;
end;

/* Procedure que usa variáveis globais */
procedure somar_globais;
var soma : integer;
begin
    soma := global1 + global2;
    global3 := soma;
end;

/* Function que retorna integer */
function quadrado(n : integer) : integer;
var res : integer;
begin
    res := n * n;
end;

/* Function com múltiplas operações */
function calcular_media(a : integer; b : integer) : integer;
var soma, media : integer;
begin
    soma := a + b;
    media := soma div 2;
end;

/* Function que usa variáveis locais */
function fatorial(n : integer) : integer;
var i, fat : integer;
begin
    fat := 1;
    i := 1;
    while i <= n do
    begin
        fat := fat * i;
        i := i + 1;
    end;
end;

/* Bloco principal */
begin
    /* Teste 1: Chamada de procedure simples */
    inicializar;
    
    /* Teste 2: Procedure com parâmetro */
    global1 := 5;
    dobrar(global1);
    write(global1);
    
    /* Teste 3: Procedure que retorna valor via parâmetro */
    calcular(resultado);
    write(resultado);
    
    /* Teste 4: Múltiplas chamadas */
    global1 := 1;
    incrementar(global1);
    incrementar(global1);
    incrementar(global1);
    write(global1);
    
    /* Teste 5: Procedure com variáveis globais */
    global1 := 10;
    global2 := 20;
    somar_globais;
    write(global3);
    
    /* Teste 6: Chamada de function */
    global1 := 5;
    
    /* Teste 7: Function em expressão */
    resultado := 10;
    
    /* Teste 8: Múltiplas procedures em sequência */
    inicializar;
    global1 := 7;
    dobrar(global1);
    incrementar(global1);
    somar_globais;
    
    write(global1);
    write(global2);
    write(global3);
end.