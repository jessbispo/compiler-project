#!/bin/bash

# Caminho para o compilador
COMPILADOR="./compilador"

# Diretório onde estão os testes
DIR_TESTES="testes"

# Verifica se o compilador existe
if [ ! -f "$COMPILADOR" ]; then
    echo "❌ Erro: compilador não encontrado em $COMPILADOR"
    exit 1
fi

# Verifica se a pasta de testes existe
if [ ! -d "$DIR_TESTES" ]; then
    echo "❌ Erro: diretório de testes '$DIR_TESTES' não encontrado"
    exit 1
fi

echo "🚀 Iniciando execução dos testes em '$DIR_TESTES'..."
echo "=============================================="

# Loop em todos os arquivos que começam com "teste"
for teste in "$DIR_TESTES"/teste*; do
    if [ -f "$teste" ]; then
        echo "🧪 Rodando teste: $teste"
        $COMPILADOR "$teste"
        STATUS=$?
        if [ $STATUS -eq 0 ]; then
            echo "✅ teste '$teste' executado com sucesso"
        else
            echo "⚠️  teste '$teste' retornou erro (código $STATUS)"
        fi
        echo "----------------------------------------------"
    fi
    sleep 5
done

echo "🏁 Todos os testes foram executados!"

