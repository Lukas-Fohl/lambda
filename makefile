CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c99 -g
SRCS=main.c expr.c read.c
TARGET=main

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET)

run: $(TARGET)
	./$(TARGET)
