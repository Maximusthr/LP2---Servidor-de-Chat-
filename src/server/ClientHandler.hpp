#ifndef CLIENT_HANDLER_HPP
#define CLIENT_HANDLER_HPP

#include <string>
#include <thread>
#include <memory> // Para std::enable_shared_from_this

class Server; 

class ClientHandler : public std::enable_shared_from_this<ClientHandler> {
public:
    //Construtor do ClientHandler.
    //socket O socket do cliente conectado.
    //server Um ponteiro BRUTO para a instância do servidor.
    ClientHandler(int socket, Server* server); 
    
    ~ClientHandler();

    //Inicia o handler, criando uma nova thread para escutar mensagens do cliente.
    void start();

    //Envia uma mensagem para este cliente.
    void sendMessage(const std::string& message);

    //Obtém o descritor do socket deste cliente.
    int getSocket() const;

private:
    void handleMessages();

    int client_socket;
    
    Server* server;
    
    std::thread handler_thread;
    bool is_running;
};

#endif