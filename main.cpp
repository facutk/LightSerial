#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
//#include "LightSerial.h"

/* mock function */
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

#define STATE_IDLE 0
#define STATE_DECODING 1

#define SIGNAL_SHORT 2
#define SIGNAL_LONG 3
#define SIGNAL_INVALID 4

class LightSerial {
  public:
    LightSerial(int led_n, int led_p);
    int available();
  private:
    void process_sample(int sample);
    void calc_threshold();
    void state_idle();
    void state_decoding();
    void store_bit(unsigned char bit);
    void log_count(int count);

    unsigned char _led_n;
    unsigned char _led_p;

    unsigned short int _samples;
    int _sample_count;

    unsigned char _window[8];
    unsigned char _slot;


    int _transition_detected;
    int _transition_count;
    int _transition_threshold;


    int _count;

    unsigned char _flanks;

    int _signal_prev;
    int _signal_cur;
    int _signal_threshold;
    int _signal_timeout;

    unsigned char _state;
    unsigned char _signal_len_prev;
    unsigned char _signal_sync_bit;

    unsigned char _byte_pos;
    unsigned char _byte;
    unsigned char _msg[64];
};

LightSerial::LightSerial(int led_n, int led_p) {
    int i;
    for (i = 0; i < 8; i++) {
        _window[i] = 0;
    }
    _samples = 0;
    _sample_count = 0;
    _slot = 0;
    _flanks = 0;
    _transition_detected = 0;
    _transition_count = 0;
    _transition_threshold = 2;
    _count = 0;
    _signal_threshold = 0;
    _signal_timeout = 255;
    _state = STATE_IDLE;
    _signal_len_prev = SIGNAL_SHORT;
    _signal_sync_bit = 0;
    _byte = 0;
    _byte_pos = 0;
}

/*  ANTESSS
int LightSerial::available() {
    _signal_cur = digitalRead(LED);

    int signal_transition = 0;
    // REVISAR ESTO!
    if (_signal_cur == _signal_prev) {
        if (_transition_detected) {
            _transition_count += 1;
            if (_transition_count >= _transition_threshold) {
                _count += _transition_count;
                _transition_count = 0;
                _transition_detected = 0;
                signal_transition = 1;
            }
        } else {
            _count += 1;
        }
    } else {
        if ( !_transition_detected) {
            _transition_detected = 1;
        } else {
            if (_transition_count < _transition_threshold) {
                _transition_detected = 0;
                _transition_count = 0;
            }
        }
    }

    switch( _state ) {
        case STATE_IDLE:
            if (signal_transition) 
                state_idle();
            break;
        case STATE_DECODING:
            if (signal_transition) 
                state_decoding();
            break;
    }
    
    _signal_prev = _signal_cur;
    return 0;
}
 *  ANTESS
*/

void LightSerial::process_sample(int sample){

    _signal_cur = sample;

    int signal_transition = 0;
    // REVISAR ESTO!
    if (_signal_cur == _signal_prev) {
        if (_transition_detected) {
            _transition_count += 1;
            if (_transition_count >= _transition_threshold) {
                _count += _transition_count;
                _transition_count = 0;
                _transition_detected = 0;
                signal_transition = 1;
            }
        } else {
            _count += 1;
        }
    } else {
        if ( !_transition_detected) {
            _transition_detected = 1;
        } else {
            if (_transition_count < _transition_threshold) {
                _transition_detected = 0;
                _transition_count = 0;
            }
        }
    }

    switch( _state ) {
        case STATE_IDLE:
            if (signal_transition) 
                state_idle();
            break;
        case STATE_DECODING:
            if (signal_transition) 
                state_decoding();
            break;
    }
    
    _signal_prev = _signal_cur;
}

int LightSerial::available() {
    int reading = digitalRead(LED);

    int signal = reading;

    _samples <<=1;
    _sample_count +=1;
    _samples += reading;
    if (_sample_count > 15) {
        int i;
        unsigned short int mask;
        for (mask = 0x8000; mask > 0; mask>>=1) {
            signal = (_samples & mask)>0;
            process_sample( signal );
        }
        _sample_count = 0;
    }

    return 0;
}

void LightSerial::state_idle() {
    _flanks +=1;
    if (_flanks >= 8) {
        calc_threshold();
        if (_count > _signal_threshold) {
            _state = STATE_DECODING;
            _signal_sync_bit = _signal_cur;
        }
    }
    log_count(_count); // we store the lenght of the changes
    _count = 1; 
}

void LightSerial::state_decoding(){
    unsigned char signal_len_cur;
    if (_count > _signal_threshold) {
        signal_len_cur = SIGNAL_LONG;
    } else {
        signal_len_cur = SIGNAL_SHORT;
    }

    //int emit = 0;
    if (_signal_len_prev == SIGNAL_SHORT && signal_len_cur == SIGNAL_SHORT) { //emit = 1;
        store_bit(_signal_sync_bit);
        signal_len_cur = SIGNAL_INVALID;
    }
    if (signal_len_cur == SIGNAL_LONG) { //emit = 1;
        _signal_sync_bit = (_signal_sync_bit == 0)?1:0;
        store_bit(_signal_sync_bit);
    } //printf("%d\t%d\t%c\t%c\n", d->count, d->signal_threshold, (d->count > d->signal_threshold) ? 'L':'S',emit?'0'+d->signal_sync_bit:'-');

    _signal_len_prev = signal_len_cur;
    _count = 1; 
}

void LightSerial::log_count(int count) {
    _window[_slot] = count;
    _slot += 1;
    if (_slot > 7) 
        _slot = 0;
}

void LightSerial::store_bit(unsigned char bit) {
    _byte <<= 1;
    _byte += bit;

    _byte_pos +=1;
    if (_byte_pos > 7) {
        _byte_pos = 0;
        printf("%c", _byte);
    }
}

void LightSerial::calc_threshold() {
    int avg_len = 0;
    int i;
    for (i = 0; i < 8; i++) 
        avg_len += _window[i];
    avg_len >>= 3; // avg divided by 8

    /* VARIANCE */
    //int std_dev = 0;
    //for (i = 0; i < 8; i++) 
        //std_dev += ( avg_len - _window[i] )*( avg_len - _window[i] );
    //std_dev >>=3;
    //std_dev = sqrt( std_dev );
    //std_dev = 3*std_dev;
    //printf("std_dev: %d\n", std_dev);
    //_transition_threshold = std_dev; // 2 std = 95% cases
    //printf("transition: %d\n", _transition_threshold);
    /* VARIANCE */

    avg_len += avg_len>>1;
    _signal_threshold = avg_len;
}

unsigned char crc_8( unsigned char in_crc, unsigned char in_data ) {
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

#define buffer_size 1024*8

class LightEncoder {
    public:
        LightEncoder();
        void encode_msg(char* msg);
        char _msg[buffer_size];
        int _base_len;
        int _noisy;
        int _msg_len;

        void put_bit(char bit); // MOVER DE NUEVO A PRIVADO!
    private:
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
        //asigno una probabilidad del 5% de error
        int error_prob = 5;
        int error = ( rand() % 100 ) <= 5;
        if (error) {
            flipbit = (rand() % signal_len/2) + signal_len/2;
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

int main(int argc, const char *argv[]) {

    //printf("%d\n", sizeof(LightSerial) );
    /*
    char stream[2048];
    sprintf( stream, 
        "100000000010000000000100000000000100000000001000"// Zero Padding
        "0000000001111111111" "0000000000001111111111"
        "000000000000111111111111111" "000000000011111111111" 
        
        "000000000000000111111111111111" "0000000000000111111111111" 
        "0000000000000011111111111" "1101111111111111000000000000"

        "111111111111111000000000010"
        "0000000000001111111111" 
        "000000000000111111111111" 
        "101111111111111100000000010""00000000011111111101111111111000000000000001111111111100000000000011111111111000000000000011111111111000000000000001000000000000001111111111111000000000000001111111111111011111111111110000000000001000000000011111111111111000000000000111111111111000000000011111111111100000000000000111111111101111111111000000000001000000000000111111111100000000000111111111111111011111100000000010000000000000001111111110000000100001111111110111111111111110000000000111111111101111000000000011111111111111000000000001000000000000111111111111110000000001001111111110111111111111000000000001111111111000000000000111111111110000000000000001111111111111000000000010000000000011111111111000000000001000000000000100000000010000000000"

                     ); // Zero Padding

    LightSerial testSerial = LightSerial(1,2);
    int j;
    for (j = 0; j < strlen(stream); j++) {
        set_pin( stream[j] );
        if ( testSerial.available() ) {
            // do something with such message
        }
    }
    printf("\n");
    return 0;

     */
    LightEncoder lenc = LightEncoder();
    lenc._base_len = 10;
    lenc._noisy = 1;

    char msg[] = "bitch better have my money!";
    lenc.encode_msg(msg);
    printf("%s\n", lenc._msg);

    LightSerial lSerial = LightSerial(1,2);
    int i;
    for (i = 0; i < lenc._msg_len; i++) {
        set_pin( lenc._msg[i] );
        if ( lSerial.available() ) {
            // do something with such message
        }
    }
    printf("\n");

    return 0;
}
