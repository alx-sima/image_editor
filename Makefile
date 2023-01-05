# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -g
LDLIBS=-lm

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
TARGET=image_editor

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

clean:
	-rm -f $(OBJS) $(TARGET) vgcore.*
