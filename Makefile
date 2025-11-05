CXX = g++
CXXFLAGS = -std=gnu++17 -O2 -Wall -Wextra -lstdc++fs
SRC = src/main.cpp src/explorer.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = file-explorer

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) $(OBJ)

