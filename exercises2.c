#include <stdio.h>
#include <stdlib.h>
int main () {
    int i, j;
    int *p[4];

    // Allocate the memory
    for (i=0; i<4; i++) {
        p[i] = (int *) malloc(4 * sizeof(int));
    }

    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            p[i][j] = i * 4 + j;
        }
    }

    // Print the array
    printf("Here's a 4x4 matrix with intergers:\n");
    for (i=0; i<4; i++) {
        for (j=0; j<4; j++) {
            printf("%2d ", p[i][j]);
        }
        printf("\n");
    }
    
    // Free the memory up
    for (i=0; i<4; i++) {
        free(p[i]);
    }
}