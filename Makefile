CC = g++
CFLAGS ?= -Wall
OFLAGS ?= -O0
LDFLAGS ?= -static

all: lightSerial

lightSerial: LightSerial.cpp main.cpp
	$(CC) LightSerial.cpp main.cpp $(OFLAGS) -o $@
	./$@
