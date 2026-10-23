CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -Iinclude -Isrc -I.

# Captura AUTOMATICAMENTE todos os .cpp dentro de src/ (incluindo o main.cpp)
SRCS = $(wildcard *.cpp) $(wildcard src/*.cpp)
OBJS = $(SRCS:.cpp=.o)

TARGET = processador_audio

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)