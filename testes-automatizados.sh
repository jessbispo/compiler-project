#!/bin/bash

COMPILADOR="./compilador"

DIR_TESTES="testes"

if [ ! -f "$COMPILADOR" ]; then
    echo "erro: compilador nao encontrado em $COMPILADOR"
    exit 1
fi
if [ ! -d "$DIR_TESTES" ]; then
    echo "erro: diretorio de testes '$DIR_TESTES' nao encontrado"
    exit 1
fi

echo "iniciando execucao dos testes em '$DIR_TESTES'..."
echo "=============================================="

for teste in "$DIR_TESTES"/teste*; do
    if [ -f "$teste" ]; then
        echo "rodando teste: $teste"
        $COMPILADOR "$teste"
        STATUS=$?
        if [ $STATUS -eq 0 ]; then
            echo "teste '$teste' executado com sucesso"
        else
            echo "teste '$teste' retornou erro (código $STATUS)"
        fi
        echo "----------------------------------------------"
    fi
    sleep 3
done

echo "todos os testes foram executados"

