#!/bin/bash

# Script de robô de chat usando netcat (nc).

# Verifica se os argumentos (IP e Porta) foram fornecidos
if [ "$#" -ne 2 ]; then
    echo "Uso: $0 <IP do Servidor> <Porta>"
    exit 1
fi

IP=$1
PORT=$2

# O bloco de comandos entre parênteses será enviado para o netcat.
# O netcat envia para o servidor.
(
    echo "Olá, eu sou o robô de script!"
    sleep 2 # Espera 2 segundos
    echo "Estou testando o servidor a partir de um script .sh."
    sleep 3 # Espera 3 segundos
    echo "Aparentemente, tudo está funcionando como esperado."
    sleep 2
    echo "Agora ficarei em modo de escuta."

    # O comando 'cat' mantém a conexão aberta
    # permitindo mensagens manualmente depois que o script terminar.
    cat
) | nc $IP $PORT