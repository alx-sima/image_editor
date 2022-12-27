# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -g
LDLIBS=-lm

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

TARGET=image_editor
IMAGE=basescu.ppm
OUTIMG=basescu3.ppm

VALGRINDFLAGS=-s --leak-check=full --track-origins=yes --show-leak-kinds=all

define CMDS
LOAD $(IMAGE)
EQUALIZE
HISTOGRAM 10 30
HISTOGRAM 5 255
EXIT
endef

build: $(TARGET)

export CMDS
test: $(TARGET) $(IMAGE)
	echo "$$CMDS" | valgrind $(VALGRINDFLAGS) ./$(TARGET)
	gimp $(OUTIMG)
	-rm -f $(OUTIMG)

export CMDS
debug: $(TARGET) $(IMAGE)
	tmux \
		new-session 'echo "$$CMDS" | valgrind $(VALGRINDFLAGS) --vgdb-error=0 ./$(TARGET)' \; \
		split-window -h 'gdb'


$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.pgm %.ppm: %.jpg
	convert $^ $@

clean:
	-rm -f $(OBJS) $(TARGET) vgcore.*
