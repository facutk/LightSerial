#ifndef ARDUINOMOCK_H
#define ARDUINOMOCK_H

extern int pin_state;
extern int LED;
/* mock function */
int digitalRead(int );
void set_pin(char );

#endif
