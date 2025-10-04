#include "Server.hpp"
#include "ClientHandler.hpp"
#include "Logger.hpp"

// Bibliotecas C padrão para Sockets
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h> // Para a função close()
#include <stdexcept> // Para std::runtime_error
#include <string.h> // Para strerror
#include <errno.h> // Para a variável errno
#include <arpa/inet.h> // Variavel inet_ntop
#include <algorithm> // Para std::remove_if

// Construtor
Server::Server(int port)
    : port(port), server_socket(-1), is_running(false) {
    Logger::getInstance().log(LogLevel::INFO, "Servidor criado na porta " + std::to_string(port));
}

// Destrutor
Server::~Server() {
    is_running = false;
    // Precisaremos de uma forma de notificar a thread acceptClients para parar
    // Por enquanto, apenas fechamos o socket.
    if (server_socket != -1) {
        close(server_socket);
    }
    Logger::getInstance().log(LogLevel::INFO, "Servidor finalizado.");
}

void Server::start() {
    // 1. Criar o socket do servidor (IPv4, TCP)
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        throw std::runtime_error("Falha ao criar o socket: " + std::string(strerror(errno)));
    }
    Logger::getInstance().log(LogLevel::INFO, "Socket do servidor criado.");

    // Para evitar o erro "Address already in use"
    int opt = 1;
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        throw std::runtime_error("Falha ao configurar SO_REUSEADDR: " + std::string(strerror(errno)));
    }

    // 2. Preparar a estrutura de endereço do servidor e fazer o bind
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // Escutar em todas as interfaces de rede
    server_addr.sin_port = htons(port); // Converter a porta para a ordem de bytes de rede

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        throw std::runtime_error("Falha ao fazer o bind na porta " + std::to_string(port) + ": " + std::string(strerror(errno)));
    }
    Logger::getInstance().log(LogLevel::INFO, "Servidor fez o bind na porta " + std::to_string(port));

    // 3. Colocar o socket em modo de escuta
    if (listen(server_socket, 10) < 0) { // 10 é o 'backlog', o número de conexões pendentes na fila
        throw std::runtime_error("Falha ao colocar o socket em modo de escuta: " + std::string(strerror(errno)));
    }

    // 4. Iniciar a thread que aceita conexões
    is_running = true;
    std::thread accepter_thread(&Server::acceptClients, this);
    accepter_thread.detach(); // A thread rodará de forma independente

    Logger::getInstance().log(LogLevel::INFO, "Servidor iniciado e escutando por conexões...");
}

void Server::acceptClients() {
    Logger::getInstance().log(LogLevel::INFO, "Thread de aceitação de clientes iniciada.");

    while (is_running) {
        sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);

        // Aceitar uma nova conexão. Esta chamada é BLOQUEANTE.
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_len);

        if (client_socket < 0) {
            // Se o servidor não estiver mais rodando, o erro é esperado.
            if (!is_running) {
                break;
            }
            Logger::getInstance().log(LogLevel::ERROR, "Falha ao aceitar conexão: " + std::string(strerror(errno)));
            continue;
        }

        // Converte o endereço IP do cliente para uma string para o log
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        Logger::getInstance().log(LogLevel::INFO, "Cliente conectado do IP: " + std::string(client_ip));

        // CRIAÇÃO DO HANDLER E ADIÇÃO À LISTA (Seção Crítica)
        {
            // Usamos lock_guard para garantir que o mutex seja liberado automaticamente
            std::lock_guard<std::mutex> lock(clients_mutex);

            // Cria um novo handler para o cliente e o adiciona à lista.
            clients.push_back(std::make_shared<ClientHandler>(client_socket, this));
            
            // Inicia a thread do novo handler para que ele comece a processar mensagens.
            clients.back()->start(); 
        }
    }

    Logger::getInstance().log(LogLevel::INFO, "Thread de aceitação de clientes finalizada.");
}

void Server::broadcastMessage(const std::string& message, int sender_socket) {
    std::lock_guard<std::mutex> lock(clients_mutex);

    // 1. Criamos um prefixo para identificar o remetente.
    std::string sender_id = "[Cliente " + std::to_string(sender_socket) + "]: ";
    
    // 2. Montamos a mensagem final que será enviada a todos.
    std::string formatted_message = sender_id + message;

    Logger::getInstance().log(LogLevel::INFO, "Fazendo broadcast da mensagem: " + formatted_message);

    for (const auto& client : clients) {
        if (client->getSocket() != sender_socket) {
            // 3. Enviamos a mensagem já formatada.
            client->sendMessage(formatted_message);
        }
    }
}

void Server::removeClient(int client_socket) {
    // Adquire o lock para modificar a lista de clientes de forma segura.
    std::lock_guard<std::mutex> lock(clients_mutex);

    // O "erase-remove idiom" é a forma padrão em C++ para remover elementos de um container.
    clients.erase(
        std::remove_if(clients.begin(), clients.end(),
            [client_socket](const std::shared_ptr<ClientHandler>& handler) {
                return handler->getSocket() == client_socket;
            }),
        clients.end()
    );

    Logger::getInstance().log(LogLevel::INFO, "Cliente no socket " + std::to_string(client_socket) + " removido da lista.");
}