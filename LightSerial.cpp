#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include "LightSerial.h"
#include "ArduinoMock.h"

void LightSerial::clean_state() {
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
    _transition_threshold = 3;
    _count = 0;
    _signal_threshold = 0;
    _signal_timeout = 64;
    _state = STATE_IDLE;
    _signal_len_prev = SIGNAL_SHORT;
    _signal_sync_bit = 0;
    _byte = 0;
    _byte_pos = 0;
}

LightSerial::LightSerial(int led_n, int led_p) {
    clean_state();
}

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
        if (_count > _signal_timeout) {
            clean_state();
            _state = STATE_TIMEOUT;
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
        case STATE_TIMEOUT:
            _state = STATE_IDLE;
            break;

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

    
    //printf("%d\n", avg_len);

    // VARIANCE
    //int std_dev = 0;
    //for (i = 0; i < 8; i++) 
        //std_dev += ( avg_len - _window[i] )*( avg_len - _window[i] );
    //std_dev >>=3;
    //std_dev = sqrt( std_dev );
    //printf("%d\n", std_dev);
    //std_dev = 3*std_dev;
    //printf("std_dev: %d\n", std_dev);
    //_transition_threshold = std_dev; // 2 std = 95% cases
    //printf("transition: %d\n", _transition_threshold);
    // VARIANCE

    avg_len += avg_len>>1;
    _signal_threshold = avg_len;
    _signal_timeout = avg_len<<=1;
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
