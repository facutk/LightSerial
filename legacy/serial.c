#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#define HIGH 1
#define LOW 0
#define BASELEN 5
#define FLIP_CHANCE 0

void delay(int microseconds) {}

int digitalRead(int pin) {
    char buf;
    read(pin, &buf, 1);
    int reading = 0;
    if (buf == '1') {
        reading = 1;
    }
    return reading;
}

void digitalWrite(int pin, uint8_t state) {
    char states[] = "01";
    int outlen = BASELEN;
    int noise_max = 1 + BASELEN/4;
    int noise = rand() % noise_max;
    outlen += noise;
    int i;
    for (i = 0; i < outlen; i++) {
        int flipbit = ( rand() % 100 ) < FLIP_CHANCE;
        if (flipbit) {
            write(pin, &states[!state], 1);
        } else {
            write(pin, &states[state], 1);
        }
    }
}

typedef struct decoder {
    int _pin;
    uint8_t _prev_reading;
    uint8_t _count;
    uint8_t _transition_count;
    uint8_t _decoding;
    uint8_t _window[8];
    uint8_t _slot;
    uint8_t _avg;
    uint8_t _bit_count;
    char _byte;
    char _byte_decoded;
    uint8_t _decoded;
} tDecoder;

void decoder_init(tDecoder* d, int pin) {
    d->_pin = pin;
    d->_prev_reading = 0;
    d->_count = 0;
    d->_decoding = 0;
    d->_slot = 0;
    d->_avg = 0;
    d->_bit_count = 0;
    d->_byte = 0x00;
    d->_decoded = 0;
    d->_transition_count = 0;
}

void decoder_decode_bit(tDecoder* d, uint8_t bit) {
    //  0  1  2  3  4  5  6  7  8  9 10
    // LO D0 D1 D2 D3 D4 D5 D6 D7 PT HI
    //printf("%d\t%d\n", d->_bit_count, bit);

    //switch( d->_bit_count ) {}
    
    if (d->_bit_count == 0) {
        if (bit == 1) {
            // ERROR
            printf("START ERROR\n");
            d->_decoding = 0;
        }
    } else if (d->_bit_count == 10) {
        if (bit == 0) {
            // ERROR
            printf("END ERROR\n");
            d->_decoding = 0;
        }
        if (d->_decoding) {
            //printf("DECODED: %c\n", d->_byte);
            d->_decoded = 1;
            d->_byte_decoded = d->_byte;
        }
    } else if (d->_bit_count == 9) {
        uint8_t parity = 1;
        unsigned char mask;
        for (mask = 0x80; mask > 0; mask >>=1) {
            if ((d->_byte & mask ) > 0) {
                parity += 1;
            }
        }
        parity = parity % 2;
        if (bit != parity) {
            //ERROR
            printf("PARITY ERROR\n");
            d->_decoding = 0;
        }
    } else {
        d->_byte <<=1;
        d->_byte += bit;
    }

    d->_bit_count += 1;
    if ( d->_bit_count > 10 ) {
        d->_bit_count = 0;
    }
}

int decoder_available(tDecoder*d) {
    int reading = digitalRead( d->_pin );

    uint8_t transition = 0;
    if (reading == d->_prev_reading) {
        if (d->_count < 255) {
            d->_count +=1;
        }
    } else {
        transition = 1;
    }

    if (transition) {
        if (d->_decoding) {
            int bit_count = d->_count / d->_avg;
            int i;
            for (i = 0; i < bit_count; i++) {
                decoder_decode_bit(d, d->_prev_reading);
            }
            //printf("count: %d, avg: %d, bit_count: %d\n", d->_count, d->_avg, bit_count);
        
        } else {
            if (d->_slot > sizeof(d->_window)) {
                int i;
                int sum = 0;
                for (i = 0; i < sizeof( d->_window ); i++) {
                    sum += d->_window[i];
                }
                int avg = sum / sizeof( d->_window );
                //printf("sum: %d, avg: %d\n", sum, avg);
                if ( d->_count >= (avg*2) ) {
                    d->_decoding = 1;
                    d->_avg = avg;
                }
            }
            d->_slot += 1;
            d->_window[ d->_slot % sizeof( d->_window ) ] = d->_count;
        }
        d->_count = 1;
    }

    d->_prev_reading = reading;
    return d->_decoded;
}

char decoder_read(tDecoder* d) {
    d->_decoded = 0;
    return d->_byte_decoded;
}

typedef struct encoder {
    int _pin;
} tEncoder;

void encoder_init(tEncoder* e, int pin) {
    e->_pin = pin;
}

void encode_bit(tEncoder* e, uint8_t state) {
    digitalWrite(e->_pin, state);
    delay(50);
}

void encode_char(tEncoder* e, char c) {
    encode_bit(e, LOW);

    uint8_t parity = 1;
    unsigned char mask;
    for (mask = 0x80; mask > 0; mask >>=1) {
        uint8_t bit = (c & mask) > 0;
        if (bit) {
            parity += 1;
        }
        encode_bit(e, bit);
    }
    parity = parity % 2;
    encode_bit(e, parity);

    encode_bit(e, HIGH);
}

void encode_msg(tEncoder* e, char * msg) {
    encode_bit(e, LOW);
    int msg_len = strlen(msg);
    int i;
    char header = 0xAA;
    encode_char(e, header);
    for (i = 0; i < msg_len; i++) {
        encode_char(e, msg[i]);
    }
    encode_bit(e, LOW);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    int pipefd[2];
    pid_t cpid;
 
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid = fork();
    if (cpid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid == 0) {    /* Child reads from pipe */
        close(pipefd[1]);          /* Close unused write end */

        tDecoder decoder;
        decoder_init(&decoder, pipefd[0]);

        int i;
        for (i = 0; i < 2048; i++) {
            if ( decoder_available(&decoder) ) {
                char buf = decoder_read(&decoder);
                write(STDOUT_FILENO, &buf, 1);
                //printf("%c", decoder_read(&decoder) );
            }
        }
        //while (read(pipefd[0], &buf, 1) > 0)
            //write(STDOUT_FILENO, &buf, 1);

        write(STDOUT_FILENO, "\n", 1);
            close(pipefd[0]);
        _exit(EXIT_SUCCESS);

    } else {
        char msg[] = "bitch";
        close(pipefd[0]);          /* Close unused read end */
        //printf("msg to encode: %s\n", msg);

        tEncoder encoder;
        encoder_init(&encoder, pipefd[1]);
        encode_msg(&encoder, msg);

        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}
