// NOTE: WINDOWS DECS ONLY. GET RID IF TESTING ON LINUX!
#define _GNU_SOURCE
#include <stdio.h>
//#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct Rule {
    char ip[2][8];
    int ports[2];
} Rule;
void printRequests(char* reqs, int pos) {
    //Loop through the request array, printing each one
    for (int i=0; i<pos; i++) {
        printf("%c\n", reqs[i]);
    }
}
void addRule(int pos, int cap, Rule* rules) {
    // Checks if we need to add more memory to the rules list.
    if (pos >= cap)
    {
        cap *= 2;
        rules = (Rule *)realloc(rules, cap * sizeof(Rule));
    }

    //Accept rule input
    //Rule newRule;
    char *inpBuf;
    size_t bufSize = 100;
    size_t inpChars;
    //char ip[];
    //char port[];

    inpBuf = (char*)malloc(bufSize * sizeof(char));

    printf("\n- Please enter a new rule in the form <IPAddress> <ports>: ");
    
    inpChars = 0;
    //Get input and check str length
    while (!inpChars == ) {
        inpChars = getline(&inpBuf, &bufSize, stdin);
        if (!inpChars == 37 && !inpChars == 18) {
            printf("\n- Rule invalid. Please try again");
        } else {
            break;
        }
    } 
    
    //Check if it's valid
    printf("\n- You inputted %s", inpBuf);
    //Add to the rules list

    //Print response

    // Free buffer
    free(inpBuf);
}


int main (int argc, char ** argv) {
    // Declare the requests list and allocate mem for 10
    char *reqs;
    int reqPos = 0, reqCap=10;
    reqs = (char*)malloc(reqCap*sizeof(char));
    // Declare the rules list and allocate memory
    Rule *rules;
    int rulePos = 0; int ruleCap=10;
    rules = (Rule*)malloc(ruleCap*sizeof(Rule));
    
    // Run the program indefinetly
    while (true) {

        //Checks if we need to add more memory to the request list.
        if (reqPos >= reqCap) {
            reqCap *= 2;
            reqs = (char*)realloc(reqs, reqCap*sizeof(char));
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
                addRule(rulePos, ruleCap, rules);
                break;
            case 'C':
                break;
        }
    }

    //Free memory when we are done
    free(reqs);
    free(rules);
}

