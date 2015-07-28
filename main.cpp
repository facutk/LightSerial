#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <getopt.h>
#include <unistd.h>
#include "LightSerial.h"
#include "ArduinoMock.h"
int debug = 0;

#define buffer_size 1024*8

class LightEncoder {
    public:
        LightEncoder();
        void encode_msg(char* msg);
        char _msg[buffer_size];
        int _base_len;
        int _noisy;
        unsigned int _msg_len;

    private:
        void put_bit(char bit);
        void encode_bit(char bit);
        void encode_char(char c);
        unsigned char crc_8( unsigned char in_crc, unsigned char in_data );
};

LightEncoder::LightEncoder(){
    srand(time(NULL));
    int i;
    for (i = 0; i < buffer_size; i++) {
        _msg[i] = 0;
    }
    _msg_len = 0;
    _base_len = 1;
    _noisy = 0;
}

void LightEncoder::put_bit(char bit) {
    int signal_len = _base_len;
    int flipbit = -1;
    if (_noisy) {
        int noise_max = 1 + _base_len / 2;
        int noise = rand() % noise_max;
        signal_len += noise;
        //asigno una probabilidad del 10% de error
        int error_probability = 10;
        int error = ( rand() % 100 ) <= error_probability;
        if (error) {
            flipbit = (rand() % (int)floor(signal_len*.9)) + signal_len*.1;
        }
    }
    int i;
    for (i = 0; i < signal_len; i++) {
        char outbit = bit;
        if (i == flipbit) {
            outbit = (outbit=='0')?'1':'0';
        }
        _msg[ _msg_len ] = outbit;
        _msg_len += 1;
    }
}

void LightEncoder::encode_bit(char bit) {
    if (bit) {
        put_bit('0');
        put_bit('1');
    } else {
        put_bit('1');
        put_bit('0');
    }
}

void LightEncoder::encode_char(char c) {
    int i;
    char mask = 0x80;
    for (i = 0; i < 8; i++) {
        char bit = (c & mask)>0;
        encode_bit(bit);
        c<<=1;
    }
}

void LightEncoder::encode_msg(char* msg) {
    // Zero padding 
    put_bit('0');
    put_bit('0');
    put_bit('0');
    put_bit('0');

    char preamble = 0xFE;
    encode_char(preamble);

    unsigned char msg_len = strlen(msg);
    unsigned char checksum = 0xFF;
    int i;
    for (i = 0; i < msg_len; i++) {
        checksum = crc_8(checksum, msg[i]);
        encode_char(msg[i]);
    }

    // CRC8
    //encode_char(e, checksum);

    // Zero padding 
    put_bit('0');
    put_bit('0');
    put_bit('0');
    put_bit('0');
    put_bit('0');
    put_bit('0');
}

unsigned char LightEncoder::crc_8( unsigned char in_crc, unsigned char in_data ) {
    unsigned char i;
    unsigned char data;
    data = in_crc ^ in_data;
    for ( i = 0; i < 8; i++ ) {
        if (( data & 0x80 ) != 0 ) {
            data <<= 1;
            data ^= 0x07;
        } else {
            data <<= 1;
        }
    }
    return data;
}

int x() {

    //unsigned short int samples;
    //printf("%d\n", sizeof( samples )<<3 );

    //printf("%d\n", sizeof(LightSerial) );
    char stream[2048];
    sprintf( stream, 


"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"

"000000000001111111"
"000000000011111111"
"00000000001111111"
"000000000111111111"

"000000000111111111"
"00000000001111111"
"00000000011111111"
"10001111110000000000"

"1111111100000000"
"000000001111111111"
"11011111100000000011111111"
"100000000000000"
"000111111111011"
"111111110000000"
"111111111100000"
"000111111111000"
"000001111111111"
"000000000000000"
"000111111110111"
/*
*/

"111110000000001"
"111111110000000"
"000000000001111"
"111000000000111"
"111111000000000"
"111111111000000"
"001111111111011"
"111111000000000"
"000000000111111"
/*
*/

"110111111111000"
"000001111111111"
"000000000000000"
"011111111110000"
"000011111111101"
"111111110000000"
"011111111110000"
"000011111111100"
"000000000000000"
"011111111011111"
"111100000000111"
"111111110000000"
"111111111000000"
"000011111111000"
"000001111111111"
"110000000000000"
"001111111110000"
"000000011111101"
"111111110000000"
"011111111111000"
"000000000000111"
"111111111001111"
"100000000000000"
"000111111111110"
"000000111111111"
"000000000111111"
"111000000000000"
/*
*/
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"
"000000000000000"

                     );

    LightSerial testSerial = LightSerial(1,2);
    unsigned int j;
    for (j = 0; j < strlen(stream); j++) {
        set_pin( stream[j] );
        if ( testSerial.available() ) {
            // do something with such message
        }
    }
    printf("\n");
    return 0;


    /*
     */

    LightEncoder lenc = LightEncoder();
    lenc._base_len = 10;
    lenc._noisy = 1;

    char msg[] = "bitch better have my money!";
    lenc.encode_msg(msg);
    printf("%s\n", lenc._msg);

    LightSerial lSerial = LightSerial(1,2);
    unsigned int i;
    for (i = 0; i < lenc._msg_len; i++) {
        set_pin( lenc._msg[i] );
        if ( lSerial.available() ) {
            // do something with such message
        }
    }
    printf("\n");

    return 0;
}

// valid short options
const char* const opCortas = "edb:nxHh:l:";

// valid long options
const struct option opLargas[] = {
    { "encode",  no_argument,       NULL, 'e' },
    { "decode",  no_argument,       NULL, 'd' },
    { "baselen", required_argument, NULL, 'b' },
    { "noise",   no_argument,       NULL, 'n' },
    { "debug",   no_argument,       NULL, 'x' },
    { "help",    no_argument,       NULL, 'H' },
    { "high",    required_argument, NULL, 'h' },
    { "low",     required_argument, NULL, 'l' },
    { NULL,      no_argument,       NULL, 0 }
};

void print_help() {
  printf("LightSerial v0.1\n");
}

int main(int argc, char** argv) {

    //x();
    //return 0;
    // getopt does not print to stderr
    opterr = 0;
    
    // flag for every argument processed
    int nextOpt = 0;
    int base_len = 3;
    int noisy = 0;

    typedef enum {PASS, ENCODE, DECODE} tAction;

    tAction action = PASS;

    while (1) {

        nextOpt = getopt_long( argc,
                               argv,
                               opCortas,
                               opLargas,
                               NULL);

        if (nextOpt == -1) {
                break;
        }

        switch (nextOpt) {

            case 'e': {
                action = ENCODE;
                break;
            }

            case 'd': {
                action = DECODE;
                break;
            }

            case 'b': {
                if (strlen(optarg) > 0) {
                    if ( ( sscanf( optarg, "%d", &base_len) != 1 ) ||
                         ( base_len <= 0 ) ) {
                        fprintf( stderr,
                                 "fatal: invalid base_len specification.\n");
                        return 1;
                    }
                }
                break;
            }

            case 'n': {
                noisy = 1;
                break;
            }

            case 'x': {
                debug = 1;
                break;
            }

            case 'H': {
                print_help();
                return 0;
                break;
            }

            default: {
                print_help();
                exit(-1);
                break;
            }

        }
    }

    switch (action) {
        case PASS: {
            break;
        }

        case ENCODE: {
                LightEncoder lenc = LightEncoder();
                lenc._base_len = base_len;
                lenc._noisy = noisy;

                //char msg[] = "bitch better have my money!";

                char buffer[4];
                unsigned int i;
                for (i = 0; i < sizeof(buffer); i++) {
                    buffer[i] = 0;
                }
                int n_read;
                while ( (n_read = read(STDIN_FILENO, buffer, sizeof(buffer))) > 0 ) {
                    buffer[n_read] = 0;
                    lenc.encode_msg(buffer);
                    write(STDOUT_FILENO, lenc._msg, lenc._msg_len);
                    for (i = 0; i < sizeof(buffer); i++) {
                        buffer[i] = 0;
                    }
                    for (i = 0; i < lenc._msg_len; i++) {
                        lenc._msg[i] = 0;
                    }
                    lenc._msg_len = 0;
                }
            break;
        }
        case DECODE: {
            LightSerial lSerial = LightSerial(1,2);
            char buffer[16];
            int n_read;
            while ( (n_read = read(STDIN_FILENO, buffer, 16)) > 0 ) {
                //buffer[n_read] = 0;
                //printf("%s\n", buffer);

                int i;
                for (i = 0; i <= n_read; i++) {
                    char state = buffer[i];
                    if (state == '0' or state == '1') {
                        set_pin( state );
                        if ( lSerial.available() ) {
                            // do something with such message
                        }
                    }
                }
            }
            printf("\n");
            break;
        }
    }
    return 0;
}

/*
 * Tengo varios niveles de complejidad juntos
 * El sampleo
 * Cuando se llena un sampleo, paso al recuento
 * Hay eventos de 0 y 1
 * Cuando hay un cambio de 0 a 1, hay una transicion
 * Despues de varias transiciones se pasa de estado
 * */
