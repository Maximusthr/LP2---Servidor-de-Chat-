# --- Compilador e Flags ---
CXX = g++
CXXFLAGS = -std=c++17 -Wall -pthread -g

# --- Diretórios ---
BIN_DIR = bin
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
SRC_ROOT = src
TEST_DIR = tests

# Lista de todos os diretórios que contêm código-fonte para o VPATH e includes
ALL_SRC_DIRS = $(SRC_ROOT)/logger $(SRC_ROOT)/server $(SRC_ROOT)/client $(SRC_ROOT) $(TEST_DIR)
VPATH = $(ALL_SRC_DIRS)

# Inclui todos os diretórios de código no include path do compilador
INCLUDE_DIRS = $(foreach dir, $(ALL_SRC_DIRS), -I$(dir))
CXXFLAGS += $(INCLUDE_DIRS)

# --- Alvos (Executáveis) ---
SERVER_EXEC = $(BIN_DIR)/chat_server
CLIENT_EXEC = $(BIN_DIR)/chat_client
TEST_EXEC = $(BIN_DIR)/test_logger # Executável de teste da Etapa 1

# --- Arquivos Fonte e Objeto ---
# Fontes para o SERVIDOR
SERVER_SRCS = server_main.cpp Server.cpp ClientHandler.cpp Logger.cpp
SERVER_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))

# Fontes para o CLIENTE
CLIENT_SRCS = client.cpp
CLIENT_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(CLIENT_SRCS))

# Fontes para o TESTE do Logger (usando o arquivo correto 'test_logger.cpp')
TEST_SRCS = test_logger.cpp Logger.cpp
TEST_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# --- Regras do Makefile ---

# A regra 'all' agora constrói o servidor e o cliente.
all: $(SERVER_EXEC) $(CLIENT_EXEC)

# Regras explícitas para construir cada componente individualmente
server: $(SERVER_EXEC)
client: $(CLIENT_EXEC)
test_logger: $(TEST_EXEC)

# Regra para linkar o executável do SERVIDOR
$(SERVER_EXEC): $(SERVER_OBJS) | $(BIN_DIR)
	@echo "==> Linkando executável do servidor: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para linkar o executável do CLIENTE
$(CLIENT_EXEC): $(CLIENT_OBJS) | $(BIN_DIR)
	@echo "==> Linkando executável do cliente: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra para linkar o executável de TESTE
$(TEST_EXEC): $(TEST_OBJS) | $(BIN_DIR)
	@echo "==> Linkando executável de teste: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra de padrão genérica para compilar qualquer arquivo .cpp em um .o
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "==> Compilando $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras de Utilidade (sem duplicação) ---
$(BIN_DIR):
	@mkdir -p $@

$(OBJ_DIR):
	@mkdir -p $@

clean:
	@echo "==> Limpando arquivos de build e executáveis..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR)

.PHONY: all clean server client test_logger