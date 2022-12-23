# Copyright Sima Alexandru 312CA (2022-2023)

CC=gcc
CFLAGS=-Wall -Wextra -g

SRCS=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRCS))

TARGET=image_editor
IMAGE=test.pgm
OUTIMG=out.pgm

define CMDS
LOAD $(IMAGE)
SELECT 0 0 200 200
CROP
SAVE $(OUTIMG) ascii
EXIT
endef

build: $(TARGET)

export CMDS
test: $(TARGET) $(IMAGE)
	echo "$$CMDS" | valgrind -s --leak-check=full --track-origins=yes ./$(TARGET)
	gimp $(OUTIMG)
	-rm -f $(OUTIMG)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.pgm %.ppm: %.jpg
	convert $^ $@

clean:
	-rm -f $(OBJS) $(TARGET) vgcore.*