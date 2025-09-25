#ifndef SERVER_HPP
#define SERVER_HPP

#include <vector>
#include <thread>
#include <mutex>
#include <string>
#include <memory> // Para std::shared_ptr
#include "Logger.hpp"

// Forward declaration para evitar dependência circular, se necessário no futuro.
class ClientHandler; 

class Server {
public:
    // Construtor do Servidor.
    // port: A porta em que o servidor irá escutar por conexões.
    Server(int port);

    // Destrutor
    ~Server();

    // Inicia a execução do servidor.
    void start();

    // Transmite uma mensagem para todos os clientes conectados, exceto um.
    // message: A mensagem a ser transmitida.
    // sender_socket: O socket do cliente que enviou a mensagem (para não reenviar para ele).
    void broadcastMessage(const std::string& message, int sender_socket);

private:
    // Loop principal para aceitar novas conexões de clientes.
    void acceptClients();
    
    // Remove um cliente da lista de clientes ativos.
    // client_socket: O socket do cliente a ser removido.
    void removeClient(int client_socket);

    int port;                     // Porta do servidor
    int server_socket;            // Socket de escuta do servidor
    bool is_running;              // Flag para controlar o loop principal
    
    // Lista de handlers de cliente. Usamos shared_ptr para gerenciar a vida dos objetos.
    std::vector<std::shared_ptr<ClientHandler>> clients; 
    
    // Mutex para proteger o acesso à lista de clientes (essencial para concorrência)
    std::mutex clients_mutex; 
};

#endif