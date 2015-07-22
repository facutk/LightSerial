#include <stdio.h>
#include <string.h>

int main(int argc, const char *argv[])
{
    
    int bit = 0;
    int was_short = 0;
    char sequence[] = "LLSSLSS";

    int i;
    for( i = 0; i < strlen( sequence ); i++ ) 
    {
        if ( sequence[i] == 'L' ) {
            if ( was_short ) {
                printf("SYNC ERROR\n");
            }
            else
            {
                printf("%d ", bit);
                bit = !bit;
                was_short = 0;
            }
        }
        else
        {
            if (was_short) {
                was_short = 0;
                printf("%d ", bit);
            }
            else
            {
                was_short = 0;
            }
        }
    }

    return 0;
}
