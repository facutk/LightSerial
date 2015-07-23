#ifndef LightSerial_h
#define LightSerial_h

#define STATE_IDLE 0
#define STATE_DECODING 1
#define STATE_TIMEOUT 2

#define SIGNAL_SHORT 2
#define SIGNAL_LONG 3
#define SIGNAL_INVALID 4

class LightSerial {
  public:
    LightSerial(int led_n, int led_p);
    int available();
  private:
    void clean_state();
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

#endif
