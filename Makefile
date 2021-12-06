TARGET = rtl_davis
LIBS = -lm $(shell pkg-config --libs librtlsdr) -pthread
CC = gcc
#CFLAGS = -g -std=gnu11 -Wall -Wextra -O3 -W $(shell pkg-config --cflags librtlsdr)
CFLAGS = -g -std=gnu11 -O3 -W $(shell pkg-config --cflags librtlsdr)

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard *.h)

%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

rtl_davis: $(OBJECTS)
	$(CC) $(OBJECTS) -Wall $(LIBS) -o $@



clean:
	-rm -f *.o
	-rm -f $(TARGET)
