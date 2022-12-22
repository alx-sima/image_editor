# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -g

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))
TARGET=image_editor

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

clean:
	-rm -f $(OBJS) $(TARGET)