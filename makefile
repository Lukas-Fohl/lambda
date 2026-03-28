CC=gcc
CFLAGS=-Wall -Wextra -pedantic -std=c2x -g
SRCS=main.c expr.c read.c
OBJS=$(SRCS:.c=.o)
TARGET=main

.PHONY: all clean run

all: $(TARGET)

main.o: main.c expr.h read.h util.h
expr.o: expr.c expr.h util.h
read.o: read.c read.h expr.h util.h

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: $(TARGET)
	./$(TARGET)
