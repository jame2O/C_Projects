#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BUFFERLENGTH 256

void error(char *msg)
{
    perror(msg);
    exit(0);
}

int main (int argc, char ** argv) {
    /* to be written */
    printf ("Client to be written\n");
    return 0;
}
