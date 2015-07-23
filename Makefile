CC = g++
CFLAGS ?= -Wall -g
OFLAGS ?= -O0
LDFLAGS ?= -static

all: lightSerial

lightSerial: ArduinoMock.cpp LightSerial.cpp main.cpp
	$(CC) ArduinoMock.cpp LightSerial.cpp main.cpp $(CFLAGS) $(OFLAGS) -o $@
	echo "bitch better have my money!" | ./$@ -e -b 10 -n | ./$@ -d
