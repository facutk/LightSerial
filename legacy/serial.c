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

void digitalWrite(int pin, uint8_t state) {
    char states[] = "01";
    int outlen = BASELEN;
    int noise_max = 1 + BASELEN/2;
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

typedef struct enconder {
    int _pin;
    int _baselen;
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
    char buf;
 
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

        while (read(pipefd[0], &buf, 1) > 0)
            write(STDOUT_FILENO, &buf, 1);

        write(STDOUT_FILENO, "\n", 1);
            close(pipefd[0]);
        _exit(EXIT_SUCCESS);

    } else {
        char msg[] = "b";
        close(pipefd[0]);          /* Close unused read end */
        printf("msg to encode: %s\n", msg);

        tEncoder encoder;
        encoder_init(&encoder, pipefd[1]);
        encode_msg(&encoder, msg);

        close(pipefd[1]);          /* Reader will see EOF */
        wait(NULL);                /* Wait for child */
        exit(EXIT_SUCCESS);
    }
}
