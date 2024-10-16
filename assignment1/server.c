#include <stdio.h>
//#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

void printRequests(char* reqs, int pos) {
    //Loop through the request array, printing each one
    for (int i=0; i<pos; i++) {
        printf("%c\n", reqs[i]);
    }
}
void addRequests(char* reqs, int nReq) {

}

int main (int argc, char ** argv) {
    // Declare the requests list and allocate mem for 10
    char *reqs;
    int reqPos = 0, capacity=10;
    reqs = (char*)malloc(capacity*sizeof(char));
    
    while (true) {

        //Checks if we need to add more memory to the request list.
        if (reqPos >= capacity) {
            capacity *= 2;
            reqs = (char*)realloc(reqs, capacity*sizeof(char));
        }

        //Accept next Request, add to list
        char nextReq;
        printf("\n- Please enter a request: ");
        scanf(" %c", &nextReq);
        reqs[reqPos] = nextReq;
        reqPos++;

        //Process the request:
        switch (nextReq) {
            case 'R':
                printRequests(reqs, reqPos);
                break;
            case 'A':
                addRequest();
                break;
            case 'C';
        }
    }
    free(reqs);
    
}

