#include "Logger.hpp"

// Implementação do Singleton
Logger& Logger::getInstance() {
    static Logger instance; // Instância estática criada na primeira chamada e destruída no final do programa.
    return instance;
}

Logger::~Logger() {
    stop(); // Garante que o logger seja parado corretamente (princípio RAII).
}

void Logger::start(const std::string& filename) {
    // Abre o arquivo de log. O modo 'app' (append) adiciona ao final do arquivo se ele já existir.
    log_file.open(filename, std::ios::out | std::ios::app);
    if (!log_file.is_open()) {
        throw std::runtime_error("Nao foi possivel abrir o arquivo de log.");
    }

    // Inicia a thread consumidora que executará a função processQueue.
    // Usamos 'this' para que a função membro possa acessar as variáveis da instância.
    writer_thread = std::thread(&Logger::processQueue, this);
}

void Logger::stop() {
    // Bloco para garantir que o lock seja liberado ao sair do escopo.
    {
        std::unique_lock<std::mutex> lock(mtx);
        done = true; // Sinaliza para a thread consumidora que ela deve terminar.
    }
    cv.notify_one(); // Acorda a thread consumidora caso ela esteja esperando por mensagens.

    // Sincroniza com a thread consumidora, esperando que ela termine sua execução.
    if (writer_thread.joinable()) {
        writer_thread.join();
    }

    // Fecha o arquivo se estiver aberto.
    if (log_file.is_open()) {
        log_file.close();
    }
}

void Logger::log(LogLevel level, const std::string& message) {
    // Formata a mensagem de log com timestamp, nível e a mensagem em si.
    std::stringstream ss;
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    ss << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X");
    ss << " [" << levelToString(level) << "] - " << message;

    // Bloco para garantir que o lock seja liberado o mais rápido possível.
    {
        std::unique_lock<std::mutex> lock(mtx);
        message_queue.push(ss.str()); // Adiciona a mensagem formatada à fila.
    }
    cv.notify_one(); // Notifica a thread consumidora que há uma nova mensagem.
}

void Logger::processQueue() {
    while (true) {
        std::unique_lock<std::mutex> lock(mtx);

        // A thread "dorme" até que a condição seja satisfeita:
        // 1. A flag 'done' seja verdadeira (sinal de parada).
        // 2. A fila de mensagens não esteja vazia.
        cv.wait(lock, [this] { return done || !message_queue.empty(); });

        // Se o sinal de parada foi recebido e a fila está vazia, podemos sair do loop.
        if (done && message_queue.empty()) {
            break;
        }

        // Para minimizar o tempo com o mutex travado, movemos as mensagens para uma fila local.
        // Isso permite que outras threads continuem adicionando logs enquanto esta escreve no arquivo.
        std::queue<std::string> local_queue;
        local_queue.swap(message_queue);

        // Liberamos o lock manualmente para permitir que outras threads produzam logs.
        lock.unlock();

        // Processa a fila local, escrevendo cada mensagem no arquivo e no console.
        while (!local_queue.empty()) {
            const std::string& msg = local_queue.front();
            if (log_file.is_open()) {
                log_file << msg << std::endl;
            }
            std::cout << msg << std::endl;
            local_queue.pop();
        }
    }
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}