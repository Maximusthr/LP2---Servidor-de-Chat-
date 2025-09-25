#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <string>
#include <thread>
#include <memory> // Para std::enable_shared_from_this e std::weak_ptr
#include "Server.hpp" // Incluímos a definição completa do Server

class ClientHandler : public std::enable_shared_from_this<ClientHandler> {
public:
    // Construtor do ClientHandler.
    // socket: O socket do cliente conectado.
    // server: Um ponteiro fraco para a instância do servidor.
    ClientHandler(int socket, std::weak_ptr<Server> server);

    // Destrutor
    ~ClientHandler();

    // Inicia o handler, criando uma nova thread para escutar mensagens do cliente.
    void start();

    // Envia uma mensagem para este cliente.
    // message: A mensagem a ser enviada.
    void sendMessage(const std::string& message);

    // Obtém o descritor do socket deste cliente.
    int getSocket() const;

private:
    // Loop principal que roda na thread para ler mensagens do cliente.
    void handleMessages();

    int client_socket;              // Socket deste cliente
    std::weak_ptr<Server> server;   // Ponteiro fraco para o servidor (evita referência circular)
    std::thread handler_thread;     // Thread de execução para este cliente
    bool is_running;                // Flag para controlar o loop da thread
};

#endif