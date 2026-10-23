CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -I.

# Pega AUTOMATICAMENTE todos os arquivos .cpp da pasta src
SRCS = $(wildcard src/*.cpp)
# Gera a lista de arquivos objeto (.o) correspondentes
OBJS = $(SRCS:.cpp=.o)

# Nome do executável final
TARGET = processador_audio

# Regra principal
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

# Regra para compilar cada .cpp em um .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Limpeza de arquivos temporários
clean:
	rm -f $(OBJS) $(TARGET)