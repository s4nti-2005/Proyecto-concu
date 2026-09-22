# Hito 1 — concurrencia en CPU
# Linux: hace falta -pthread. macOS suele aceptarlo igual.
CXX      ?= c++
CXXFLAGS ?= -std=c++17 -O2 -Wall -Wextra
LDFLAGS  ?= -pthread

all: secuencial paralelo benchmark

secuencial: secuencial.cpp
	$(CXX) $(CXXFLAGS) -o $@ $<

paralelo: paralelo.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

benchmark: benchmark.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $<

clean:
	rm -f secuencial paralelo benchmark

.PHONY: all clean
