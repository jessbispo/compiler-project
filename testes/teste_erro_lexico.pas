/* Teste de erro léxico - caractere inválido */
program erro_lexico;
var a, b : integer;

begin
    a := 10;
    b := 20;
    
    /* A linha abaixo contém um caractere inválido @ */
    a @ b;
    
    /* O compilador deve parar aqui e reportar erro léxico */
end.