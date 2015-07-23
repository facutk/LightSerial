CC = g++
CFLAGS ?= -Wall -g
OFLAGS ?= -O0
LDFLAGS ?= -static

all: lightSerial

lightSerial: LightSerial.cpp main.cpp
	$(CC) LightSerial.cpp main.cpp $(CFLAGS) $(OFLAGS) -o $@
	echo "bitch" | ./$@ -e -b 10 -n | ./$@ -d
