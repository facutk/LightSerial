#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

#define DOT_LEN 4

char lookup[] = " ETIANMSURWDKGOHVF L@PJBXCYZQ-*54.3,:|2!?+#$&'16=/()<>[7]{}8_90";

char decode( char * code ) 
{
    int index = 0;
    int code_len = strlen( code );
    int i;
    int increment = 0;
    for (i = 0; i < code_len; i++) 
    {
        index *= 2;
        increment = ( code[i] == '.')? 1 : 2;
        index += increment;
    }
    return lookup[ index ];
}

void tests()
{
    char MorseL[]=".-..";
    assert( decode( MorseL ) == 'L' );
}

typedef enum {
    state_none,
    state_dot,
    state_dash,
    state_intersymbol,
    state_interchar,
    state_interword
} State;

typedef enum {
    signal_low,
    signal_high
} Signal;

State state = state_none;
Signal current_signal = signal_low;

int state_count;
int symbol_index;
bool interchar_done = false;

void update_state( Signal signal )
{

    if ( state == state_none ) 
    {
        interchar_done = false;
        if ( signal == signal_low )
        {
            state_count = 0;
        } 
        else 
        {
            state = state_dot;
            state_count = 0;
            symbol_index = 0;
        }
    }

    else if ( state == state_dot )
    {
        if ( signal == signal_low )
        {
            symbol_index *= 2;
            symbol_index += 1;
            state_count = 0;
            state = state_intersymbol;
        }
        else
        {
            if ( state_count > DOT_LEN*2 ) {
                state = state_dash;
            }
        }
    } 

    else if ( state == state_dash )
    {
        if ( signal == signal_low )
        {
            symbol_index *= 2;
            symbol_index += 2;
            state_count = 0;
            state = state_intersymbol;
        }
    } 

    else if ( state == state_intersymbol )
    {
        if ( signal == signal_low )
        {
            if ( state_count > DOT_LEN*2 ) {
                state = state_interchar;
            }
        }
        else
        {
            state = state_dot;
        }
    } 

    else if ( state == state_interchar )
    {
        if ( signal == signal_low )
        {
            if ( !interchar_done ) {
                interchar_done = true;
                if ( state_count > DOT_LEN*2 ) {
                    symbol_index = (symbol_index < sizeof(lookup))? symbol_index : 0;
                    printf("%c", lookup[symbol_index]);
                    state = state_none;
                }
            }
            if ( state_count > DOT_LEN*7 ) {
                state = state_interword;
                interchar_done = false;
            }
        }
        else
        {
            symbol_index = 0;
            state = state_dot;
        }
    } 

    else if ( state == state_interword )
    {
        printf(" ");
        state = state_none;
    }


    state_count += 1;
}


int main(int argc, const char *argv[])
{
    //tests();

    //char reading[] = "1111001001111000000000000000001011100000100000000000000";
    char reading[] = "000000000000000000000000000000111100001111000000111111111100000011100000000000000001110000001111111111000000000000000011111111111100001110000001111111111000000111000000000000000001100000111100000111111111110000000000000000011111111110000000000000000111111111111000011100000011111111111000000000000000000000000000000";


    int i;
    for (i = 0; i < strlen( reading ); i++) {
        Signal s = (reading[i] == '0')? signal_low: signal_high;
        update_state( s );
        //printf("%d\n", state);
        //printf("%d\n", symbol_index );
    }

    //char code[]=".-..";
    //printf("%c\n", decode( code ));
    /*
    char buf[1];
    ssize_t nread;
    while ( ( nread = read( STDIN_FILENO, buf, 1 ) ) >0 ) {
        write( STDOUT_FILENO, buf, nread );
    }
    */
    return 0;
}
