CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall
SRC = src/city_navigator.cpp
BIN = city_navigator

all: $(BIN)

$(BIN): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(BIN) $(SRC)

run: $(BIN)
	./$(BIN)

clean:
	rm -f $(BIN)

.PHONY: all run clean
