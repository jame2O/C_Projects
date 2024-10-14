#include <stdio.h>
#include <stdlib.h>

int main()
{
    int a = 0x4321;
    int *b = &a;
    printf("Ptr -> %d\n", *b);

    int c[2] = {222, 333};
    int *d = c;
    printf("Array ptr -> %d\n", *d);

    char e[] = "Hello, OpSys";
    char *f = e + 3;
    printf("String ptr -> %c\n", *e);

    int *much = malloc(10000*sizeof(int));
    if(much == NULL) exit(1);
    printf("malloc()ed... ");
    // do stuff
    free(much);
    printf("free()d.\n");

    return 0;
}