/* Teste de estruturas de controle IF e WHILE */
program estruturas;
var i, soma, max : integer;
var continua, encontrado : boolean;

begin
    /* Teste 1: IF simples */
    i := 5;
    if i > 0 then
        soma := 10;
    
    /* Teste 2: IF-THEN-ELSE */
    i := 3;
    if i > 5 then
        soma := 100
    else
        soma := 50;
    
    /* Teste 3: IF com bloco BEGIN-END */
    if i < 10 then
    begin
        soma := i + 5;
        i := i + 1;
        write(soma);
    end;
    
    /* Teste 4: IF-ELSE com blocos */
    if i = 0 then
    begin
        soma := 0;
        write(soma);
    end
    else
    begin
        soma := 1;
        write(i);
    end;
    
    /* Teste 5: WHILE simples */
    i := 0;
    while i < 5 do
        i := i + 1;
    
    /* Teste 6: WHILE com bloco */
    i := 0;
    soma := 0;
    while i < 10 do
    begin
        i := i + 1;
        soma := soma + i;
    end;
    
    /* Teste 7: IF aninhado */
    i := 5;
    max := 10;
    if i > 0 then
    begin
        if i < max then
        begin
            soma := i * 2;
            write(soma);
        end
        else
            soma := max;
    end;
    
    /* Teste 8: WHILE aninhado */
    i := 0;
    while i < 3 do
    begin
        soma := 0;
        while soma < 5 do
        begin
            soma := soma + 1;
        end;
        i := i + 1;
    end;
    
    /* Teste 9: IF-ELSE encadeado */
    i := 7;
    if i < 3 then
        soma := 1
    else
        if i < 7 then
            soma := 2
        else
            soma := 3;
    
    /* Teste 10: Combinação IF e WHILE */
    i := 1;
    soma := 0;
    continua := true;
    while continua do
    begin
        soma := soma + i;
        i := i + 1;
        if i > 10 then
            continua := false;
    end;
    
    write(soma);
end.