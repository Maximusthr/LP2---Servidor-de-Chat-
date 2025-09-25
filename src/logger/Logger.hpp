#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

// Enum para os níveis de log
enum class LogLevel {
    INFO,
    WARNING,
    ERROR
};

class Logger {
public:
    // O padrão Singleton nos dá um ponto de acesso global.
    // Deletamos o construtor de cópia e o operador de atribuição para evitar múltiplas instâncias.
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    // Obtém a instância única do Logger.
    static Logger& getInstance();

    // Inicia o sistema de logging, definindo o arquivo de saída e iniciando a thread consumidora.
    // filename: nome do arquivo onde os logs serão salvos.
    void start(const std::string& filename);

    // Para o sistema de logging de forma segura.
    void stop();

    // Adiciona uma mensagem de log à fila. Este método é thread-safe.
    // level: O nível do log (INFO, WARNING, ERROR).
    // message: A mensagem a ser registrada.
    void log(LogLevel level, const std::string& message);

private:
    // Construtor e destrutor privados para o padrão Singleton.
    Logger() : done(false) {}
    ~Logger();

    // --- Membros da Fila Thread-Safe (Padrão Monitor) ---
    std::queue<std::string> message_queue; // A fila de mensagens.
    std::mutex mtx;                        // O mutex para proteger o acesso à fila.
    std::condition_variable cv;            // A variável de condição para notificar a thread consumidora.
    bool done;                             // Flag para sinalizar o término da thread.

    // --- Thread Consumidora e Arquivo de Log ---
    std::thread writer_thread; // A thread que escreve os logs.
    std::ofstream log_file;    // O arquivo de log.

    // Função que será executada pela thread consumidora.
    void processQueue();

    // Converte o nível de log para uma string.
    // level: O nível do log.
    std::string levelToString(LogLevel level);
};

#endif