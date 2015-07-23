#include "ArduinoMock.h"

int pin_state;
int LED;
int digitalRead(int pin) {
    return pin_state;
}
void set_pin(char value) {
    if (value == '0') {
        pin_state = 0;
    } else {
        pin_state = 1;
    }
}

