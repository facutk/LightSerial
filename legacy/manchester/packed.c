#include <stdio.h>
#include <stdint.h>

typedef struct Denoise {
  uint8_t ready      :1;
  uint8_t working    :1;
  uint8_t transition :3;
  uint8_t threshold  :3;
  uint8_t count;
} tDenoise;

int main(int argc, const char *argv[])
{
    printf("%d\n", sizeof(tDenoise));
    return 0;
}
