#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define MAX_PORT 65535
#define MAX_IP 255

typedef struct Rule {
    char ip[2][8];
    int ports[2];
} Rule;

int validateIp(char *ip) {
    //IP Validation
    printf("Validating");
    int dotCount;
    while (*ip) {
        if (*ip == '.') {
            dotCount++;
        };
        ip++;
    }
    if (dotCount != 3) return -1;
    char* nextSplit = strtok(ip, ".");
    //Check if numbers are valid
    while (nextSplit != NULL) {
        if (atoi(nextSplit) > MAX_IP || atoi(nextSplit) < 0 || atoi(nextSplit) == 0) return -1;
        nextSplit = strtok(NULL, ".");
    }
    return 1;
    
}
int compareIps(char* ip1, char* ip2) {
    char
    int inet_aton(const char *cp, struct in_addr *ip1);
}
int validatePort(char *port) {
    //Port Validation
    if (atoi(port) < 0 || atoi(port) > MAX_PORT) return -1;
    return 1;
}

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
    if (inpChars) {
        printf("willy");
    } // Make gcc happy 
    printf("\n- Please enter a new rule in the form <IPAddress> <ports>: ");
    inpChars = getline(&inpBuf, &bufSize, stdin);
    printf("check");
    //Split IP & Port & check validation
    char ips[50], ports[50];
    if (sscanf(inpBuf, "%s %s", ips, ports) != 2) {
        printf("\nInvalid rule");
        return 0;
    }
    //Check the IP and split if nescessary, then verify 
    if (strchr(ips, '-')) { //Multiple case
        char ip1[20], ip2[20];
        sscanf(ips, "%[^-]-%s", ip1, ip2);
        if (validateIp(ip1) == -1 || validateIp(ip2) == -1) {
            printf("\nInvalid rule");
            return 0;
        } else {
            if (compareIps(ip1, ip2) == -1) {
                printf("\nInvalid rule");
                return 0;
            }
        }
    } else {
        if (validateIp(ips) == -1) {
            printf("\nInvalid rule");
            return 0;
        }
    }
    //Check the port and split if nescessary, then verify
    if (strchr(ports, '-')) { //Multiple case
            char port1[10], port2[20];
            sscanf(ports, "%[^-]-%s", port1, port2);
            if (validatePort(port1) == -1 || validatePort(port2) == -1 || port2 >= port1) {
                printf("\nInvalid rule");
                return 0;
            }
        } else {
            if (validatePort(ports) == -1) {
                printf("\nInvalid rule");
                return 0;
            }
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
    while (1) {

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

