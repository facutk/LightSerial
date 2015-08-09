#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* mock function */
int pin_state;
int LED;
int digitalRead(int pin) {
    return pin_state;
}
void set_pin(char value)
{
    //printf("%c\t%d\n", value, count);
    if (value == '0') {
        pin_state = 0;
    } else {
        pin_state = 1;
    }
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


#define buffer_size 1024
typedef struct encoder_t {
    char msg[buffer_size];
    short msg_len;
    int base_len;
    int noisy;
} tEncoder ;

void put_bit(tEncoder*e, char bit) {
    int signal_len = e->base_len;
    if (e->noisy) {
        int noise_max = 1 + e->base_len / 2;
        int noise = rand() % noise_max;
        signal_len += noise;
    }
    int i;
    for (i = 0; i < signal_len; i++) {
        e->msg[ e->msg_len ] = bit;
        e->msg_len += 1;
    }
}

void encode_bit(tEncoder* e, char bit) {
    if (bit) {
        put_bit(e, '0');
        put_bit(e, '1');
    } else {
        put_bit(e, '1');
        put_bit(e, '0');
    }
}

void encode_char( tEncoder* e, char c) {
    int i;
    char mask = 0x80;
    for (i = 0; i < 8; i++) {
        char bit = (c & mask)>0;
        encode_bit(e, bit);
        c<<=1;
    }
}

void encoder_init( tEncoder * e ) {
    srand(time(NULL));
    int i;
    for (i = 0; i < buffer_size; i++) {
        e->msg[i] = 0;
    }
    e->msg_len = 0;
    e->base_len = 1;
    e->noisy = 0;
}

void encode_msg(tEncoder* e, char* msg) {

    // Zero padding 
    put_bit(e, '0');
    put_bit(e, '0');
    put_bit(e, '0');
    put_bit(e, '0');

    char preamble = 0xFE;
    encode_char(e, preamble);

    unsigned char msg_len = strlen(msg);
    unsigned char checksum = 0xFF;
    int i;
    for (i = 0; i < msg_len; i++) {
        checksum = crc_8(checksum, msg[i]);
        encode_char( e, msg[i] );
    }

    // CRC8
    //encode_char(e, checksum);

    // Zero padding 
    put_bit(e, '0');
    put_bit(e, '0');
    put_bit(e, '0');
    put_bit(e, '0');
}

typedef struct {
    unsigned char led_n;
    unsigned char led_p;

    unsigned char window[8];
    unsigned char slot;

    int count;

    unsigned char flanks;

    int signal_prev;
    int signal_cur;
    int signal_threshold;
    unsigned char state;
    unsigned char signal_len_prev;
    unsigned char signal_sync_bit;

    unsigned char byte_pos;
    unsigned char byte;
    unsigned char msg[3];
} tDecoder;

#define STATE_IDLE 0
#define STATE_DECODING 1

#define SIGNAL_SHORT 2
#define SIGNAL_LONG 3
#define SIGNAL_INVALID 4
void decoder_init(tDecoder* d) {
    int i;
    for (i = 0; i < 8; i++) {
        d->window[i] = 0;
    }
    d->slot = 0;
    d->flanks = 0;
    d->count = 0;
    d->signal_threshold = 0;
    d->state = STATE_IDLE;
    d->signal_len_prev = SIGNAL_SHORT;
    d->signal_sync_bit = 0;
    d->byte = 0;
    d->byte_pos = 0;
}

void decoder_calc_threshold(tDecoder* d) {
    int avg_len = 0;
    int i;
    for (i = 0; i < 8; i++) 
        avg_len += d->window[i];
    /*
    printf("<THRESHOLD>\n");
    for (i = 0; i < 8; i++) {
        printf("%d\n",d->window[i]);
    }
    printf("</THRESHOLD>\n");
    */
    avg_len >>= 3; // avg divided by 8
    //printf("%d\n", avg_len);
    
    avg_len += avg_len>>1;
    //d->signal_threshold = avg_len * 1.6; // *2 * 80%
    d->signal_threshold = avg_len;
}

void decoder_log_count(tDecoder* d, int count) {
    d->window[d->slot] = count;
    d->slot += 1;
    if (d->slot > 7) 
        d->slot = 0;
}

void decoder_store_bit(tDecoder* d, unsigned char bit) {
    d->byte <<= 1;
    d->byte += bit;

    d->byte_pos +=1;
    if (d->byte_pos > 7) {
        d->byte_pos = 0;
        printf("BYTE: %x\n", d->byte);
    }
}

void decoder_state_decoding(tDecoder* d){
    unsigned char signal_len_cur;
    //printf("\n"); printf("->>>>>>%d\n", d->count);
    if (d->count > d->signal_threshold) {
        signal_len_cur = SIGNAL_LONG;
    } else {
        signal_len_cur = SIGNAL_SHORT;
    }

    //int emit = 0;
    if (d->signal_len_prev == SIGNAL_SHORT && signal_len_cur == SIGNAL_SHORT) {
        //emit = 1;
        //printf("%d", d->signal_sync_bit); //store_bit( signal_bit_sync );
        decoder_store_bit(d, d->signal_sync_bit);
        signal_len_cur = SIGNAL_INVALID;
    }
    if (signal_len_cur == SIGNAL_LONG) {
        //emit = 1;
        d->signal_sync_bit = (d->signal_sync_bit == 0)?1:0;
        //printf("%d", d->signal_sync_bit);// store_bit( signal_bit_sync );
        decoder_store_bit(d, d->signal_sync_bit);
    }
    //printf("%d\t%d\t%c\t%c\n", d->count, d->signal_threshold, (d->count > d->signal_threshold) ? 'L':'S',emit?'0'+d->signal_sync_bit:'-');

    d->signal_len_prev = signal_len_cur;
    d->count = 1; 
}

void decoder_state_idle(tDecoder* d) {
    // we store the lenght of the changes
    decoder_log_count(d, d->count);
    //printf("-------> %d <---------\n", d->count);
    d->flanks +=1;
    if (d->flanks >= 8) {
        decoder_calc_threshold(d);
        //printf("=======>> %d <===========\n", d->signal_threshold);
        if (d->count > d->signal_threshold) {
            //printf("DECODING\n");
            d->state = STATE_DECODING;
            d->signal_sync_bit = d->signal_cur;
        }
    }
    d->count = 1; 
}

int decoder_msg_arrived(tDecoder* d) {
    d->signal_cur = digitalRead(LED);
    //printf("%d\n", d->signal_cur);

    int signal_transition = 0;
    if (d->signal_cur == d->signal_prev) {
        d->count += 1;
    } else {
        signal_transition = 1;
    }

    switch( d->state ) {
        case STATE_IDLE:
            //printf("%d", d->signal_cur);
            if (signal_transition) 
                decoder_state_idle(d);
            break;
        case STATE_DECODING:
            if (signal_transition) 
                decoder_state_decoding(d);
            break;
    }
    
    d->signal_prev = d->signal_cur;
    return 0;
}

int main(int argc, const char *argv[]) {
    tEncoder enc;
     encoder_init(&enc);
    enc.base_len = 6;
    enc.noisy = 1;
    
    char msg[3];
    msg[0] = 0xc1;
    msg[1] = 0x9c;
    //msg[1] = 0xFF;
    msg[2] = 0x00;
    encode_msg(&enc, msg);
    printf("%s\n", enc.msg);



    tDecoder deco;
    decoder_init(&deco);
    int i;
    for (i = 0; i < enc.msg_len; i++) {
    //for (i = 0; i < strlen(stream); i++) {
        set_pin( enc.msg[i] );
        //set_pin( stream[i] );
        if ( decoder_msg_arrived(&deco) ) {
            // do something with such message
        }
    }
    printf("\n");

    return 0;
}

    /*
    char stream[512];
    sprintf( stream, "0000000000000000000000000000000" // Zero Padding
                     "0000000011111111" "00000000011111111" "00000011111111"   "000000011111111"   // 01 01 01 01 -> 1111 -> F -> Preamble High
                     "000000001111111"  "0000001111111"     "0000000011111111" "11111111110000000" // 01 01 01 10 -> 1110 -> E -> Preamble Low
                     
                     "0000000001111111" "111111100000000" "111111111000000000" "00000000011111111" // 01 10 10 01 -> 1001 -> 9
                     "000000000111111"  "0000001111111"   "11111110000000"     "000000001111111"   // 01 01 10 01 -> 1101 -> d

                     "0000000000000000000000000000000"); // Zero Padding

     */
    /*
    char preamble[] = "00000011100011100011100011100011100011100011111100000";
    int x;
    for (x = 0; x < sizeof(preamble)-1; x++) {

        set_pin( preamble[x] );
        if ( decoder_msg_arrived(&deco) ) {
            // do something with such message
        }

    }
    printf("\n");
    return 0;
    */
    //return run_tests();
    
    /*
    return 0;
    char stream[] =
        //"0110001111000001111110000000111111110000000001";
        "00000000000000000000111110000011110000111000011110000011110001";
        //"010101010101010101010101010101010";
    int size = sizeof(stream);
    signal_len = 0;
    */
    
    /*
    Exec = S_Idle;
    printf("IDLE\n");

    for (i = 0; i < 512; i++) {
        bit_buffer[i] = 0;
    }
    bit_cursor = 0;
    for (i = 0; i < enc.msg_len; i++) {
        set_pin( enc.msg[i] );
        //Exec();
        CCom();
    }
    printf("\n");

    printf("bit_buffer\n");
    printf("%s\n", bit_buffer);

    encoder_init(&enc);
    encode_char(&enc, 'h');
    printf("%s\n", enc.msg);
    return 0;
}
     */



/*
#include "minunit.h"
int tests_run = 0;

#define signal_delta_unidle 2
#define signal_count_unidle 4

typedef enum { STR, INT } tType;
typedef struct {
    tType typ;          // typ is separate.
    union {
        int ival;       // ival and sval occupy same memory.
        char *sval;
    };
} tVal;

typedef struct ledCom_t {
    int x;
} ledCom;
typedef void State;
typedef State (*Pstate)();

Pstate Exec;

State S_Idle();
State S_Decoding();

char signal_prev;
int signal_len;

int signal_avg_len;
int signal_bit_sync;
typedef enum { S_SHORT, S_LONG } tSlen;
tSlen sig_len_prev;
char bit_buffer[512];
void store_bit( int bit ){
    char bit_to_save;
    if (bit) {
        bit_to_save = '1';
    } else {
        bit_to_save = '0';
    }
    bit_buffer[bit_cursor]=bit_to_save;

    bit_cursor += 1;
}

State S_Decoding(){
    int signal_threshold = signal_avg_len * 1.6;

    printf("%d\t%d\t%d\t%c\n", signal_len, signal_avg_len, signal_threshold, 
           (signal_len > signal_threshold) ? 'L':'S'
            );
    tSlen sig_len_cur;
    if (signal_len > signal_threshold) {
        sig_len_cur = S_LONG;
    } else {
        sig_len_cur = S_SHORT;
    }
    if (sig_len_prev == S_SHORT && sig_len_cur == S_SHORT) {
        printf("EMIT -----> %d\n", signal_bit_sync);
        store_bit( signal_bit_sync );
    }
    if (sig_len_cur == S_LONG) {
        signal_bit_sync = (signal_bit_sync == 0)?1:0;
        printf("EMIT -----> %d\n", signal_bit_sync);
        store_bit( signal_bit_sync );
    }

    sig_len_prev = sig_len_cur;
}

int signal_len_prev;
int signal_count;
int signal_header_sum;
State S_Idle(){
    // idle until 8 consecutive signals differ no more than a delta 
    if ( abs( signal_len - signal_len_prev) < signal_delta_unidle ) {
        signal_count+=1;
        signal_header_sum += signal_len;
    } else {
        signal_count = 0;
        signal_header_sum = 0;
    }
    printf("%d\n",signal_count);
    if (signal_count >= 4) {
        Exec = S_Decoding;
        printf("DECODING\n");
        signal_avg_len = signal_header_sum / signal_count_unidle;
        printf("avg_len: %d\n", signal_avg_len);
        signal_bit_sync = signal_prev;
    }

    signal_len_prev = signal_len;

}

void CCom(){
    int signal_cur = digitalRead(LED);
    if (signal_cur == signal_prev) {
        signal_len += 1;
    } else {
        //printf("%d",signal_len);
        Exec();
        signal_len = 1;
    }
    signal_prev = signal_cur;
}


 
static char * test_foo() {
    int foo = 7;
    mu_assert("error, foo != 7", foo == 7);
    return 0;
}

static char * test_bar() {
    int bar = 4;
    mu_assert("error, bar != 5", bar == 5);
    return 0;
}

static char * all_tests() {
    mu_run_test(test_foo);
    mu_run_test(test_bar);
    return 0;
}

int run_tests() {
    char *result = all_tests();
    if (result != 0) {
        printf("%s\n", result);
    } else {
        printf("ALL TESTS PASSED\n");
    }
    printf("Tests run: %d\n", tests_run);
    return result != 0;
}
*/
