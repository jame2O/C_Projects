#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>

#define MAX_PORT 65535
#define MAX_IP 255

typedef struct Rule {
    char ip[2][16];
    int ports[2];
    char queries[10];
} Rule;

int validateIp(char *ip) {

    char ipCopy[16];
    strncpy(ipCopy, ip, 16);
    ipCopy[15] = '\0'; 
    //IP Validation
    int dotCount = 0;
    while (*ip) {
        if (*ip == '.') {
            dotCount++;
        };
        ip++;
    }
    if (dotCount != 3) return -1;
    char* nextSplit = strtok(ipCopy, ".");
    //Check if numbers are valid
    while (nextSplit != NULL) {
        for (char *ptr = nextSplit; *ptr; ptr++) {
            if (!isdigit(*ptr)) return -1; // Ensure all characters are digits
        }
        if (atoi(nextSplit) > MAX_IP || atoi(nextSplit) < 0 || atoi(nextSplit) == 0) return -1;
        nextSplit = strtok(NULL, ".");
    }
    return 1;
    
}
int compareIps(char* ip1, char* ip2) {
    struct in_addr add1, add2;
    
    //Convert the IPS to binary
    if (inet_aton(ip1, &add1) == 0 || inet_aton(ip2, &add2) == 0) {
        return -1;
    }

    if (add1.s_addr < add2.s_addr) {
        return 1;
    } else return -1;

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

int addRule(char inp[], int pos, int cap, Rule* rules) {
    // Checks if we need to add more memory to the rules list.
    if (pos >= cap)
    {
        cap *= 2;
        rules = (Rule *)realloc(rules, cap * sizeof(Rule));
    }
    //Split IP & Port & check validation
    char ips[50], ports[50];
    if (sscanf(inp, "%s %s", ips, ports) != 2) {
        printf("Invalid rule\n");
        return 0;
    }
    //Check the IP and split if nescessary, then verify, and add to the rules list
    if (strchr(ips, '-')) { //Multiple case
        char ip1[20], ip2[20];
        sscanf(ips, "%[^-]-%s", ip1, ip2);
        if (validateIp(ip1) == -1 || validateIp(ip2) == -1) {
            printf("Invalid rule\n");
            return 0;
        } else {
            if (compareIps(ip1, ip2) == -1) {
                printf("Invalid rule\n");
                return 0;
            } else { //Add to rules list
                strcpy(rules[pos].ip[0], ip1);
                strcpy(rules[pos].ip[1], ip2);
            }
        }
    } else { //Single case
        if (validateIp(ips) == -1) {
            printf("Invalid rule\n");
            return 0;
        } else {
            strcpy(rules[pos].ip[0], ips);
            strcpy(rules[pos].ip[1], "#");
        }
    }
    //Check the port and split if nescessary, then verify, and add to the rules list
    if (strchr(ports, '-')) { //Multiple case
            char port1[10], port2[20];
            sscanf(ports, "%[^-]-%s", port1, port2);
            if (validatePort(port1) == -1 || validatePort(port2) == -1 || port2 <= port1) {
                printf("Invalid rule\n");
                return 0;
            } else {
                rules[pos].ports[0] = atoi(port1);
                rules[pos].ports[1] = atoi(port2);
            }
        } else { //Single case
            if (validatePort(ports) == -1) {
                printf("Invalid rule\n");
                return 0;
            } else {
                rules[pos].ports[0] = atoi(ports);
                rules[pos].ports[1] = -1;
            }
        }

    //Print response
    printf("Rule added\n");
    // Free mallocs & return
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
        char inp[50]; 
        scanf("%c", &nextReq);
        scanf(" %[^\n]", inp);
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
                addRule(inp, rulePos, ruleCap, rules);
                break;
            case 'C':
                //query(rules);
                break;
            default:
                printf("Invalid request\n");
                break;
        }
    }

    //Free memory when we are done
    free(reqs);
    free(rules);
}

