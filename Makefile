CXX      = g++
TARGET   = processador_audio
BUILD    = build

# -MMD -MP: gera arquivos .d com as dependencias dos headers (editar um .hpp recompila o que depende dele)
CXXFLAGS = -Wall -Wextra -std=c++17 -MMD -MP -Iinclude -Iinclude/filtros $(EXTRA)

# Captura todos os .cpp: raiz (main.cpp), src/ e src/filtros/
SRCS = $(wildcard *.cpp) $(wildcard src/*.cpp) $(wildcard src/filtros/*.cpp)
OBJS = $(patsubst %.cpp,$(BUILD)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

# compila cada .cpp para build/<mesmo caminho>.o (cria a pasta se nao existir)
$(BUILD)/%.o: %.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@

debug:
	$(MAKE) BUILD=build-debug TARGET=processador_audio_debug EXTRA="-g -O0 -fsanitize=address,undefined" processador_audio_debug

# make run -> compila e executa
run: $(TARGET)
	./$(TARGET)

clean:
	rm -rf build build-debug $(TARGET) processador_audio_debug

-include $(DEPS)

.PHONY: all debug run clean