# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -std=c99
LDLIBS=-lm

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)
TARGET=image_editor

build: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

pack:
	zip -FSr 312CA_SimaAlexandru_Tema3.zip README Makefile *.c *.h

clean:
	-rm -f $(OBJS) $(TARGET) vgcore.*
