#include <stdio.h>
#include <unistd.h>

int main(int argc, const char *argv[])
{
	ssize_t n;
	char c[1];
	while ( ( n = read(0,c,1)) > 0 ) {
            write(1,c,1);
	}
	return 0;
}
