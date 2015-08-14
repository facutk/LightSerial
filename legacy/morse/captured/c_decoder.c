#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
    int fd = open("morse_limpio.txt", O_RDONLY);
    if (fd) {
        char reading;
        while (read(fd,&reading,1)) {
            printf("%c", reading);
        };
        close(fd);
    }
    return 0;
}
