CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic

SERVER_SRC := Server-Side/main.cpp
CLIENT_SRC := Client-Side/main.cpp

TARGETS := server client

.PHONY: all clean

all: $(TARGETS)

server: $(SERVER_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

client: $(CLIENT_SRC)
	$(CXX) $(CXXFLAGS) $< -o $@

clean:
	rm -f $(TARGETS)