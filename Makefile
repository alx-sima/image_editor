# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -g
LDLIBS=-lm

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

TARGET=image_editor
IMAGE=test.pgm
OUTIMG=out.pgm

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
	echo "$$CMDS" | valgrind -s --leak-check=full --track-origins=yes --show-leak-kinds=all ./$(TARGET)
	#gimp $(OUTIMG)
	-rm -f $(OUTIMG)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

%.pgm %.ppm: %.jpg
	convert $^ $@

clean:
	-rm -f $(OBJS) $(TARGET) vgcore.*