# ==============================================================================
# MAKEFILE - PROJETO NEWS TOOLKIT (C++ com Python Embedding via Pybind11)
# ==============================================================================

# 1. Compilador e Padrão C++
CXX      := g++
CXX_STD  := -std=c++17

# 2. Configurações do Python e Pybind11
# --cflags: Busca os caminhos dos .h (headers)
# --ldflags --embed: Busca as bibliotecas para linkagem.
# O flag --embed é vital para Python 3.8+
PYTHON_CFLAGS  := $(shell python3-config --cflags)
PYTHON_LDFLAGS := $(shell python3-config --ldflags --embed)
PYBIND_INCLUDES := $(shell python3 -m pybind11 --includes)

# 3. Flags de Compilação (Compile Flags)
# -Wall -Wextra: Ativa avisos úteis para debugar o código
# -O2: Otimização de performance
CXXFLAGS := $(CXX_STD) -Wall -Wextra -O2 $(PYTHON_CFLAGS) $(PYBIND_INCLUDES)

# 4. Flags de Linkagem (Linker Flags)
LDFLAGS  := $(PYTHON_LDFLAGS)

# 5. Definição dos Arquivos
TARGET   := NewsToolkitCLI
SRCS     := main.cpp NewsManager.cpp PythonBridge.cpp
OBJS     := $(SRCS:.cpp=.o)
HEADERS  := NewsStructure.h NewsManager.h PythonBridge.h

# ==============================================================================
# REGRAS DE COMPILAÇÃO
# ==============================================================================

# Regra padrão (o que acontece quando digita apenas 'make')
all: $(TARGET)

# Regra para criar o Executável Final
$(TARGET): $(OBJS)
	@echo "Linkando o executavel $(TARGET)..."
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "Sucesso! Execute com ./$(TARGET)"

# Regra para transformar arquivos .cpp em .o (Objetos)
%.o: %.cpp $(HEADERS)
	@echo "Compilando $<..."
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Regra de Limpeza (apaga arquivos temporários)
clean:
	@echo "Limpando arquivos de build..."
	rm -f $(OBJS) $(TARGET)

# Regra auxiliar para rodar o projeto
run: $(TARGET)
	./$(TARGET)

.PHONY: all clean run