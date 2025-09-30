/* Teste de expressões aritméticas, relacionais e lógicas */
program expressoes;
var a, b, c, resultado : integer;
var x, y, z : boolean;

begin
    /* === EXPRESSÕES ARITMÉTICAS === */
    
    /* Teste 1: Operadores básicos */
    a := 10;
    b := 5;
    c := 3;
    
    resultado := a + b;
    resultado := a - b;
    resultado := a * b;
    resultado := a div b;
    
    /* Teste 2: Precedência de operadores */
    resultado := a + b * c;
    resultado := a * b + c;
    resultado := a + b + c;
    resultado := a - b - c;
    resultado := a * b * c;
    resultado := a div b div c;
    
    /* Teste 3: Parênteses */
    resultado := (a + b) * c;
    resultado := a * (b + c);
    resultado := (a + b) * (c + 2);
    resultado := ((a + b) * c) div 2;
    
    /* Teste 4: Expressões com sinal */
    resultado := -a;
    resultado := -a + b;
    resultado := -(a + b);
    resultado := -a * b;
    
    /* Teste 5: Expressões complexas */
    resultado := a + b * c - 10;
    resultado := (a + b) * c div 2 + 5;
    resultado := a * b + c * 2 - 1;
    
    /* === EXPRESSÕES RELACIONAIS === */
    
    /* Teste 6: Operadores relacionais */
    x := a > b;
    x := a < b;
    x := a >= b;
    x := a <= b;
    x := a = b;
    x := a <> b;
    
    /* Teste 7: Comparações com constantes */
    x := a > 0;
    x := b < 100;
    x := c = 5;
    x := resultado <> 0;
    
    /* === EXPRESSÕES LÓGICAS === */
    
    /* Teste 8: Operadores lógicos básicos */
    x := true;
    y := false;
    z := x and y;
    z := x or y;
    z := not x;
    z := not y;
    
    /* Teste 9: Combinações lógicas */
    x := true;
    y := false;
    z := x and y;
    x := y or z;
    y := not x;
    z := x and not y;
    
    /* Teste 10: Expressões mistas */
    a := 10;
    b := 20;
    x := a > b;
    y := a < b;
    z := x or y;
    
    x := a = 10;
    y := b = 20;
    z := x and y;
    
    /* Teste 11: Expressões em comandos */
    if a > b then
        resultado := a
    else
        resultado := b;
    
    x := a > 0;
    while x do
    begin
        a := a - 1;
        x := a > 0;
    end;
    
    /* Teste 12: Expressões aninhadas */
    resultado := (a + (b * c)) div 2;
    resultado := ((a + b) * (c - 1)) div (a + 1);
    
    x := a > 0;
    y := b < 100;
    z := x and y;
    
    if z then
        resultado := 1
    else
        resultado := 0;
    
    write(resultado);
end.