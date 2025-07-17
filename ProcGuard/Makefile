CC = g++
CFLAGS = -Wall -Wextra -O2 -std=c++17
TARGET = procguard
SRC = procguard.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET)
