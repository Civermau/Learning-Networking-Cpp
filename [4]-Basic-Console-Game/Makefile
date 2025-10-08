CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic

SRCS := Server-Side/server.cpp Client-Side/client.cpp main.cpp
TARGET := game

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)