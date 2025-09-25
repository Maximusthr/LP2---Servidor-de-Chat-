// Teste de estresse

#include "Logger.hpp"
#include <vector>
#include <thread>
#include <string>

/*
Função que será executada por cada thread.
Ela simula um "trabalhador" que gera logs.
*/
void log_worker(int thread_id) {
    // Cada thread vai escrever 100 mensagens de log.
    for (int i = 0; i < 100; ++i) {
        std::string message = "Log message " + std::to_string(i) + " from thread " + std::to_string(thread_id);

        // Usamos o ID da thread para variar o nível do log.
        // Para testar os diferentes formatos de nível.
        LogLevel level = static_cast<LogLevel>(thread_id % 3);

        Logger::getInstance().log(level, message);
    }
}

int main() {
    std::cout << "Iniciando teste de estresse do Logger..." << std::endl;

    try {
        // Inicia o logger. Os logs serão salvos em "test_log.txt".
        Logger::getInstance().start("test_log.txt");

        const int NUM_THREADS = 10;
        std::vector<std::thread> threads;
        for (int i = 0; i < NUM_THREADS; ++i) {
            threads.emplace_back(log_worker, i + 1);
        }
        std::cout << NUM_THREADS << " threads foram criadas e estao escrevendo logs..." << std::endl;

        // Log de mensagem da thread principal
        Logger::getInstance().log(LogLevel::INFO, "Mensagem da thread principal (main).");

        // Espera das threads
        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }
        std::cout << "Todas as threads terminaram." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "ERRO: " << e.what() << std::endl;
        // Garante que o logger pare mesmo em caso de erro.
        Logger::getInstance().stop();
        return 1;
    }

    // Para o logger.
    // Isso garante que todas as mensagens na fila sejam escritas antes do programa fechar.
    std::cout << "Parando o logger..." << std::endl;
    Logger::getInstance().stop();

    std::cout << "Teste concluido. Verifique o arquivo 'test_log.txt'." << std::endl;

    return 0;
}
