#include <iostream>
#include <string>
#include <thread>
#include <vector>

// Bibliotecas de Rede C
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>

// Flag para controlar o loop de recebimento
bool keep_running = true;


//Função que roda em uma thread separada para receber mensagens do servidor.
//client_socket -> O socket do cliente.
void receiveMessages(int client_socket) {
    char buffer[4096];
    while (keep_running) {
        memset(buffer, 0, sizeof(buffer));
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);

        if (bytes_received > 0) {
            // Imprime a mensagem recebida diretamente no console
            std::cout << std::string(buffer, bytes_received);
        } else if (bytes_received == 0) {
            std::cout << "-> Conexão perdida com o servidor." << std::endl;
            keep_running = false;
            break;
        } else {
            // Se keep_running for false, o erro é esperado porque fechamos o socket.
            if (keep_running) {
                std::cout << "-> Erro no recv(). Desconectando." << std::endl;
                keep_running = false;
            }
            break;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Uso: " << argv[0] << " <IP do Servidor> <Porta>" << std::endl;
        return 1;
    }

    const char* server_ip = argv[1];
    int port = std::stoi(argv[2]);

    // 1. Criar o socket do cliente
    int client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0) {
        std::cerr << "Falha ao criar o socket." << std::endl;
        return 1;
    }

    // 2. Configurar o endereço do servidor
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

    // 3. Conectar ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Falha ao conectar ao servidor." << std::endl;
        close(client_socket);
        return 1;
    }

    std::cout << "-> Conectado ao servidor de chat! Você pode começar a enviar mensagens." << std::endl;

    // 4. Iniciar a thread de recebimento de mensagens
    std::thread receiver_thread(receiveMessages, client_socket);

    // 5. Loop para enviar mensagens (na thread principal)
    std::string line;
    while (keep_running) {
        std::getline(std::cin, line);

        if (!keep_running) break;

        if (line == "/quit") {
            keep_running = false;
        }
        
        if (send(client_socket, line.c_str(), line.length(), 0) < 0) {
            std::cerr << "Falha ao enviar mensagem." << std::endl;
            keep_running = false;
        }
    }

    // 6. Limpeza
    close(client_socket); // Fechar o socket fará com que recv() na outra thread pare.
    if (receiver_thread.joinable()) {
        receiver_thread.join();
    }

    std::cout << "-> Você foi desconectado." << std::endl;
    return 0;
}