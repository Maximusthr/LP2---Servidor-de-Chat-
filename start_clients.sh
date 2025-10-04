#!/bin/bash

# Script para simular múltiplos clientes se conectando ao servidor de chat.

# --- Configurações ---
NUM_CLIENTS=3 # Quantos clientes você quer iniciar.
# ---------------------

# Verifica se os argumentos (IP e Porta) foram fornecidos
if [ "$#" -ne 2 ]; then
    echo "Uso: $0 <IP do Servidor> <Porta>"
    exit 1
fi

IP=$1
PORT=$2
EXECUTABLE="./bin/chat_client"

echo "Iniciando $NUM_CLIENTS clientes para conectar em $IP:$PORT..."

# Loop para iniciar cada cliente em uma nova janela de terminal.
# A linha de comando exata pode variar dependendo do seu emulador de terminal.
# Escolha a que funciona para você ou adicione a sua.

for (( i=1; i<=$NUM_CLIENTS; i++ ))
do
    # Para GNOME Terminal (padrão do Ubuntu)
    gnome-terminal --title="Cliente $i" -- $EXECUTABLE $IP $PORT

    # Para Konsole (padrão do KDE)
    # konsole --new-tab -e $EXECUTABLE $IP $PORT &

    # Para Xterm (básico e amplamente disponível)
    # xterm -T "Cliente $i" -e $EXECUTABLE $IP $PORT &
done

echo "$NUM_CLIENTS clientes foram iniciados em novas janelas."