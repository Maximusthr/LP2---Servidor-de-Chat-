# Sistema de Chat - Projeto LPII

Sistema de chat cliente-servidor multi-threaded desenvolvido em C++ para a disciplina de Linguagem de Programação II (LPII).

## 📋 Status do Projeto

### ✅ Etapa 1: Arquitetura e Logging (Concluída)

  - **Sistema de Logging Thread-Safe**: Biblioteca `libtslog` implementada e testada.
  - **Arquitetura Inicial**: Definição das classes `Server` e `ClientHandler` e diagramas de sequência.

### ✅ Etapa 2: Protótipo Funcional (Concluída)

  - **Servidor TCP Funcional**: Implementação da lógica de `socket`, `bind`, `listen` e `accept`.
  - **Cliente CLI Funcional**: Implementação de um cliente de terminal que envia e recebe mensagens.
  - **Comunicação em Rede**: Mensagens enviadas por um cliente são retransmitidas (broadcast) para os demais.
  - **Gerenciamento de Clientes**: Servidor gerencia a conexão, desconexão e identificação de múltiplos clientes.
  - **Logging Integrado**: Eventos do servidor (conexões, mensagens) são registrados pela `libtslog`.

### ✅  Etapa 3: Finalização e Relatório (Concluída)

  - Refinamento do código e tratamento de casos extremos.
  - Elaboração do relatório final, incluindo a análise de concorrência com IA.

## 🏗️ Arquitetura

O projeto segue uma arquitetura modular composta por:

### Componentes Principais

1.  **Logger**: Sistema de logging thread-safe (`libtslog`).
2.  **Server**: Orquestra a lógica do chat, aceita conexões e gerencia o broadcast.
3.  **ClientHandler**: Objeto com thread dedicada para gerenciar a comunicação com um único cliente.
4.  **Cliente**: Aplicação de terminal que se conecta ao servidor, envia e recebe mensagens.

*(O diagrama de sequência detalhado está no arquivo `docs/ARQUITETURA.md`)*

## 🧪 Testes e Execução

### Como Compilar

O `Makefile` foi configurado para compilar o servidor e o cliente.

```bash
# Compila ambos, servidor e cliente
make all

# Compila apenas o servidor
make server

# Compila apenas o cliente
make client
```

### Como Executar

É necessário ter dois ou mais terminais abertos.

1.  **Inicie o Servidor (em um terminal):**
    ```bash
    ./bin/chat_server <PORTA>
    # Exemplo: ./bin/chat_server 8080
    ```
2.  **Inicie os Clientes (em outros terminais):**
    ```bash
    ./bin/chat_client <IP_DO_SERVIDOR> <PORTA>
    # Exemplo: ./bin/chat_client 127.0.0.1 8080
    ```

### Scripts de Simulação

Para facilitar os testes, dois scripts estão disponíveis:

1.  **Simulação de Múltiplos Clientes (`start_clients.sh`):**
    Abre várias janelas de terminal, cada uma com um cliente.
    ```bash
    # Primeiro, torne-o executável (apenas uma vez)
    chmod +x start_clients.sh

    # Inicie o servidor (em um terminal)
    ./bin/chat_server 8080

    # Execute para abrir 3 clientes
    ./start_clients.sh 127.0.0.1 8080
    ```
2.  **Robô de Chat Automatizado (`bot.sh`):**
    Conecta um cliente que envia mensagens pré-programadas e depois fica escutando.
    ```bash
    # Primeiro, torne-o executável (apenas uma vez)
    chmod +x bot.sh

    # Inicie o servidor (em um terminal)
    ./bin/chat_server 8080

    # Execute o robô
    ./bot.sh 127.0.0.1 8080
    ```

## 📁 Estrutura do Projeto

```
├── src/                          # Código fonte principal
│   ├── logger/                   # Sistema de logging
│   │   ├── Logger.hpp
│   │   └── Logger.cpp
│   ├── server/                   # Lógica do servidor de chat
│   │   ├── Server.hpp
│   │   ├── Server.cpp
│   │   ├── ClientHandler.hpp
│   │   └── ClientHandler.cpp
│   ├── client/                   # Lógica do cliente de chat
│   │   └── client.cpp
│   └── server_main.cpp           # Ponto de entrada (main) do 
├── tests/                        # Códigos de teste
│   └── test_logger.cpp           # Teste de estresse para o Logger
├── docs/                         # Documentação
│   └── ARQUITETURA.md
|   └── Diagrama-Arquitetura.png
|── logs/                         # Logs gerados
├── build/                        # (Ignorado pelo Git)
├── bin/                          # (Ignorado pelo Git)
├── .gitignore
├── Makefile
├── README.md
|── bot.sh                        # Script de teste
├── start_clients.sh              # Script de teste
└── LICENSE
```

## 📚 Padrões de Design Utilizados

1.  **Singleton Pattern** (`Logger`): Garante uma instância única e acesso global ao sistema de logging.
2.  **Monitor Pattern (Producer-Consumer)** (`Logger`): Coordena o acesso concorrente à fila de logs de forma segura e eficiente.
3.  **RAII (Resource Acquisition Is Initialization)** (`Server`, `ClientHandler`): Garante o gerenciamento automático de recursos como sockets e threads, prevenindo vazamentos.



## 📹 Extra: Demonstração em Vídeo

Um vídeo curto de demonstração foi gravado para mostrar o sistema em funcionamento, incluindo a inicialização do servidor, a conexão de múltiplos clientes e o broadcast de mensagens em tempo real.

**[>> Assista ao vídeo da demonstração aqui <<](https://youtu.be/pN5Hb6fvhfk)**
