# --- Compilador e Flags ---
# Define o compilador C++
CXX = g++
# Flags de compilação:
# -std=c++17: Habilita o padrão C++17.
# -Wall: Mostra todos os avisos (warnings).
# -pthread: Linka a biblioteca de threads.
# -g: Adiciona símbolos de depuração (debug).
CXXFLAGS = -std=c++17 -Wall -pthread -g

# --- Diretórios ---
# Diretório para executáveis finais.
BIN_DIR = bin
# Diretório para arquivos de build intermediários.
BUILD_DIR = build
# Diretório específico para arquivos objeto (.o).
OBJ_DIR = $(BUILD_DIR)/obj

# Diretórios de código fonte.
SRC_ROOT = src
SRC_DIRS = $(SRC_ROOT)/logger $(SRC_ROOT)/server
# VPATH informa ao Make onde procurar por pré-requisitos (arquivos .cpp).
VPATH = $(SRC_ROOT) $(SRC_DIRS)

# Adiciona todos os diretórios de código fonte ao include path do compilador (-I).
INCLUDE_DIRS = $(foreach dir, $(SRC_DIRS), -I$(dir)) -I$(SRC_ROOT)

# Adiciona os includes às flags do compilador.
CXXFLAGS += $(INCLUDE_DIRS)

# --- Alvos (Executáveis) ---
# Nosso executável de teste para o Logger.
TEST_EXEC = $(BIN_DIR)/test_logger

# (Futuro) O executável principal do servidor.
# SERVER_EXEC = $(BIN_DIR)/chat_server

# --- Arquivos Fonte e Objeto ---
# Fontes para o teste do logger: main.cpp e Logger.cpp.
TEST_SRCS = main.cpp Logger.cpp
# Mapeia os nomes dos arquivos fonte para seus respectivos arquivos objeto.
TEST_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(TEST_SRCS))

# (Futuro) Fontes para o servidor principal.
# SERVER_SRCS = Server.cpp ClientHandler.cpp Logger.cpp main_server.cpp
# SERVER_OBJS = $(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SERVER_SRCS))

# --- Regras do Makefile ---

# A regra 'all' é a padrão. Ela irá construir o nosso teste por enquanto.
all: $(TEST_EXEC)

# Regra para linkar o executável de teste a partir dos seus arquivos objeto.
$(TEST_EXEC): $(TEST_OBJS) | $(BIN_DIR)
	@echo "==> Linkando executável de teste: $@"
	$(CXX) $(CXXFLAGS) -o $@ $^

# (Futuro) Regra para linkar o executável do servidor.
# $(SERVER_EXEC): $(SERVER_OBJS) | $(BIN_DIR)
# 	@echo "==> Linkando executável do servidor: $@"
# 	$(CXX) $(CXXFLAGS) -o $@ $^

# Regra de padrão genérica para compilar qualquer arquivo .cpp em um .o.
# O Make usará o VPATH para encontrar o .cpp correspondente.
$(OBJ_DIR)/%.o: %.cpp | $(OBJ_DIR)
	@echo "==> Compilando $<"
	$(CXX) $(CXXFLAGS) -c $< -o $@

# --- Regras de Utilidade ---

# Regra para criar o diretório de binários se ele não existir.
# Isso é uma "order-only prerequisite", só é executada se o diretório não existir.
$(BIN_DIR):
	@echo "==> Criando diretório de binários: $@"
	@mkdir -p $@

# Regra para criar o diretório de objetos se ele não existir.
$(OBJ_DIR):
	@echo "==> Criando diretório de build: $@"
	@mkdir -p $@

# Regra 'clean' para remover todos os arquivos gerados.
clean:
	@echo "==> Limpando arquivos de build e executáveis..."
	@rm -rf $(BIN_DIR) $(BUILD_DIR)

# Declara que 'all' e 'clean' não são nomes de arquivos.
.PHONY: all clean