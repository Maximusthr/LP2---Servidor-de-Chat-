#include "ClientHandler.hpp"
#include "Server.hpp" 
#include "Logger.hpp"


#include <unistd.h> // close()
#include <sys/socket.h> // recv(), send()
#include <string.h> // strerror
#include <errno.h>

// Construtor: usa a lista de inicialização
ClientHandler::ClientHandler(int socket, Server* server)
    : client_socket(socket), server(server), is_running(false) {}

// Destrutor: garante que a thread termine e o socket seja fechado
ClientHandler::~ClientHandler() {
    close(client_socket);
    Logger::getInstance().log(LogLevel::INFO, "Handler para o socket " + std::to_string(client_socket) + " destruído.");
}

void ClientHandler::start() {
    is_running = true;
    // Inicia a thread que executará o loop de mensagens
    handler_thread = std::thread(&ClientHandler::handleMessages, this);
    handler_thread.detach(); // A thread agora é independente
}

int ClientHandler::getSocket() const {
    return client_socket;
}

void ClientHandler::sendMessage(const std::string& message) {
    // Adiciona uma nova linha para que o cliente possa ler mensagens completas
    std::string formatted_message = message + "\n";
    send(client_socket, formatted_message.c_str(), formatted_message.length(), 0);
}

void ClientHandler::handleMessages() {
    char buffer[4096]; // Buffer para receber os dados

    while (is_running) {
        // Zera o buffer antes de cada leitura
        memset(buffer, 0, sizeof(buffer));

        // A chamada recv() é BLOQUEANTE: a thread para aqui até receber dados.
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received > 0) {
            // Mensagem recebida com sucesso
            std::string message(buffer, bytes_received);
            Logger::getInstance().log(LogLevel::INFO, "Mensagem recebida do socket " + std::to_string(client_socket) + ": " + message);

            // Repassa a mensagem para o servidor fazer o broadcast
            server->broadcastMessage(message, client_socket);

        } else if (bytes_received == 0) {
            // Cliente desconectou (conexão fechada)
            Logger::getInstance().log(LogLevel::INFO, "Cliente no socket " + std::to_string(client_socket) + " desconectou.");
            is_running = false; // Sinaliza para sair do loop
            server->removeClient(client_socket); // Pede ao servidor para remover este handler
            break;

        } else {
            // Ocorreu um erro na recepção
            if (!is_running) break; // Erro esperado se o servidor estiver desligando
            
            Logger::getInstance().log(LogLevel::ERROR, "Erro no recv() para o socket " + std::to_string(client_socket) + ": " + std::string(strerror(errno)));
            is_running = false;
            server->removeClient(client_socket); // Pede ao servidor para remover este handler
            break;
        }
    }
}