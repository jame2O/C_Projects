// NOTE: WINDOWS DECS ONLY. GET RID IF TESTING ON LINUX!
#define _GNU_SOURCE
#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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
int addRule(int pos, int cap, Rule* rules) {
    // Checks if we need to add more memory to the rules list.
    if (pos >= cap)
    {
        cap *= 2;
        rules = (Rule *)realloc(rules, cap * sizeof(Rule));
    }

    //Accept rule input
    char *inpBuf = NULL;
    size_t bufSize = 0;
    size_t inpChars;

    inpBuf = (char*)malloc(bufSize * sizeof(char));
    inpChars = 0;

    printf("\n- Please enter a new rule in the form <IPAddress> <ports>: ");
    inpChars = getline(&inpBuf, &bufSize, stdin);
    
    //IP Checks
    char *end = inpBuf;
    char *start = inpBuf;
    //Split IP & Port
    while (*end != ' ' && *end != '\0') {
        end++;
    }
    char* ip = (char*)malloc((end-start)+1)
    strncpy(ip, start, (end-start));
    start = end;
    while (*end != '\0') {
        end++;
    }
    char* port = (char*)malloc((end-start)+1)
    strncpy(port, start, (end-start));

    //IP Validation
    start = ip;
    end = ip;
    while (*end != '\0') {
        while (*end != '-' && *end != '\0') {
            end++;
        }
        int ipLen = end - start;
        if (ipLen != 15) { //Length Check
            printf("\nInvalid rule");
            return 0;
        }
        char* token = (char*)malloc(tokLen + 1);
        strncpy(token, start, tokLen);

        if ((atoi(token) == 0) || (atoi(token) < 0) || (atoi(token) > 255)) { //Num check
            printf("\nInvalid rule");
            return 0;
        }
        
        free(token);
    }
    //Add to the rules list

    //Print response
    
    // Free mallocs & return
    free(inpBuf);
    return 0;
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
        scanf("%c", &nextReq);
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF); 
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

