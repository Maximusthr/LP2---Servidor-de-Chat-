#include "Server.hpp"
#include "Logger.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Uso: " << argv[0] << " <Porta>" << std::endl;
        return 1;
    }

    try {
        int port = std::stoi(argv[1]);
        
        // Inicia o logger antes de tudo
        Logger::getInstance().start("server_log.txt");

        Server server(port);
        server.start();

        // Mantém o programa principal rodando enquanto o servidor opera em threads
        std::cout << "Servidor em execução. Pressione Enter para sair." << std::endl;
        std::cin.get();

    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::ERROR, "Erro fatal: " + std::string(e.what()));
        std::cerr << "Erro fatal: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}