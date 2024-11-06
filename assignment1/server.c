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
#define BUFFERLENGTH 256

typedef struct Rule {
    char ip[2][16]; //2 spaces for IPs, 16 chars for each IP
    int ports[2]; // 2 spaces for ports
    char** queries;
    int queryCount;
    int queryCap;
} Rule;

void error(char *msg)
{
    perror(msg);
    exit(1);
}

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
        rules[pos].queryCount = 0;
        rules[pos].queryCap = 10; // Initial capacity for queries
        rules[pos].queries = (char **)malloc(rules[pos].queryCap * sizeof(char *));
        for (int i = 0; i < rules[pos].queryCap; i++)
        {
            rules[pos].queries[i] = (char *)malloc(30 * sizeof(char)); // Allocate space for each query
        }
    // Print response
    printf("Rule added\n");
    return 1;
}
int queryRule(char inp[], Rule* rules, int ruleCount) {

    // Split IP & Port & check validation
    char ip[50], port[50];
    if (sscanf(inp, "%s %s", ip, port) != 2)
    {
        printf("Illegal IP address or port specified\n");
        return -1;
    }
    // Check the IP 
    if (validateIp(ip) == -1)
    {
        printf("Illegal IP address or port specified\n");
        return -1;
    }
    // Check the port
    if (validatePort(port) == -1)
    {
        printf("Illegal IP address or port specified\n");
        return -1;
    }
    //Checks against rules
    for (int i=0; i<ruleCount; i++) {
        //Check IP First
        if ((rules[i].ip[1][0] == '#')) { //Single IP case
            if (!(strcmp(rules[i].ip[0], inp) == 0))
            {
                printf("Single Ip comparison failed\n");
                continue;
            }
        } else { //Multiple IP case
            if (!(compareIps(rules[i].ip[0], ip) == 1 && compareIps(inp, rules[i].ip[1]) == 1))
            {
                printf("mulitple Ip comparison failed\n");
                continue;
            }
        }
        //Now check ports
        if (rules[i].ports[1] != -1) { //Multiple port case
            if (!(atoi(port) >= rules[i].ports[0] && atoi(port) <= rules[i].ports[1])) {
                printf("multiple port comparison failed\n");
                continue;
            }
        } else { //Single port case
            if (!(atoi(port) == rules[i].ports[0])) {
                printf("Single port comparison failed\n");
                continue;
            }
        }
        //All checks passed, add to queries
        printf("Connection accepted\n");
        if (rules[i].queryCount >= rules[i].queryCap)
        {
            rules[i].queryCap *= 2;
            rules[i].queries = (char **)realloc(rules[i].queries, rules[i].queryCap * sizeof(char *));
            for (int j = rules[i].queryCount; j < rules[i].queryCap; j++)
            {
                rules[i].queries[j] = (char *)malloc(30 * sizeof(char));
            }
        }
        strcpy(rules[i].queries[rules[i].queryCount], inp);
        rules[i].queryCount++;
        return 1;
        
    }
    printf("Connection rejected\n");
    return -1;
}
char* rejoinRule(Rule rule, char* result)
{
    char ipPart[50];
    char portPart[20];

    // Rejoin IP part
    if (rule.ip[1][0] == '#')
    { // Single IP case
        snprintf(ipPart, sizeof(ipPart), "%s", rule.ip[0]);
    }
    else
    { // Multiple IP case
        snprintf(ipPart, sizeof(ipPart), "%s-%s", rule.ip[0], rule.ip[1]);
    }

    // Rejoin port part
    if (rule.ports[1] == -1)
    { // Single port case
        snprintf(portPart, sizeof(portPart), "%d", rule.ports[0]);
    }
    else
    { // Multiple port case
        snprintf(portPart, sizeof(portPart), "%d-%d", rule.ports[0], rule.ports[1]);
    }

    // Combine IP and port parts into result
    snprintf(result, 70, "%s %s", ipPart, portPart);
    return result;
}
int deleteRule(char inp[], Rule* rules, int ruleCount) {
    // Split IP & Port & check validation
    char ips[50], ports[50];
    if (sscanf(inp, "%s %s", ips, ports) != 2)
    {
        printf("Rule invalid\n");
        return 0;
    }
    if (strchr(ips, '-'))
    { // Multiple case
        char ip1[20], ip2[20];
        sscanf(ips, "%[^-]-%s", ip1, ip2);
        if (validateIp(ip1) == -1 || validateIp(ip2) == -1)
        {
            printf("Rule invalid\n");
            return 0;
        }
        else
        {
            if (compareIps(ip1, ip2) == -1)
            {
                printf("Rule invalid\n");
                return 0;
            }
        }
    }
    else
    { // Single case
        if (validateIp(ips) == -1)
        {
            printf("Rule invalid\n");
            return 0;
        }
    }
    if (strchr(ports, '-'))
    { // Multiple case
        char port1[10], port2[20];
        sscanf(ports, "%[^-]-%s", port1, port2);
        if (validatePort(port1) == -1 || validatePort(port2) == -1 || port2 <= port1)
        {
            printf("Rule invalid\n");
            return 0;
        }
    }
    else
    { // Single case
        if (validatePort(ports) == -1)
        {
            printf("Rule invalid\n");
            return 0;
        }
    }
    //Find the matching rule and delete it
    for (int i = 0; i < ruleCount; i++)
    {
        char ruleString[70];
        rejoinRule(rules[i], ruleString);
        if (strcmp(ruleString, inp) == 0)
        {
            for (int j = 0; j < rules[i].queryCap; j++)
            {
                free(rules[i].queries[j]);
            }
            free(rules[i].queries);
            for (int j = i; j < ruleCount - 1; j++)
            {
                rules[j] = rules[j + 1];
            }
            printf("Rule deleted\n");
            return 1;
        }
    }
    printf("Rule invalid\n");
    return -1;
}
int listRules(Rule* rules, int ruleCount) {
    for (int i=0; i<ruleCount; i++) {
        char* ruleString = (char*)malloc(70*sizeof(char));
        rejoinRule(rules[i], ruleString);
        printf("Rule: %s\n", ruleString);
        for (int j=0; j<(rules[i].queryCount); j++) {
            if (rules[i].queries[j][0] != '\0') {
                printf("Query: %s\n", rules[i].queries[j]);
            }
        }
        free(ruleString);
    }
    return 1;
}

// Main Loop for the server program

int main (int argc, char ** argv) {
    int mode = 0;
    socklen_t clilen;
    int sockfd, newsockfd, portno;
    char buffer[BUFFERLENGTH];
    struct sockaddr_in6 serv_addr, cli_addr;
    int n;
    if (argc >= 2) {
        if (strcmp(argv[1], "-i") == 0) {
            mode = 0;
        } else {
            if (atoi(argv[1]) > 0 && atoi(argv[1]) <= MAX_PORT ) {
                mode = 1;
                /* create socket */
                sockfd = socket (AF_INET6, SOCK_STREAM, 0);
                if (sockfd < 0) {
                    error("ERROR opening socket");
                }
                bzero ((char *) &serv_addr, sizeof(serv_addr));
                portno = atoi(argv[1]);
                serv_addr.sin6_family = AF_INET6;
                serv_addr.sin6_addr = in6addr_any;
                serv_addr.sin6_port = htons (portno);
                /* bind it */
                if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
                    error("ERROR on binding");
                }
                /* ready to accept connections */
                listen(sockfd,5);
                clilen = sizeof (cli_addr);
            } else {
                printf("Invalid argument\n");
                return 0;
            }
        }
    }
    
    // Declare the requests list and allocate mem for 10
    int ruleCount =0;
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
        char nextReq;
        char inp[50]; 
        if (mode == 1) {
            /* waiting for connections */
            newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen);
            if (newsockfd < 0) {
                error ("ERROR on accept");
            }
            bzero (buffer, BUFFERLENGTH);
            /* read the data */
            n = read (newsockfd, buffer, BUFFERLENGTH - 1);
            if (n < 0) {
                error ("ERROR reading from socket");
            }
            printf ("Here is the message: %s\n", buffer);
            
        } else {
            // Interactive mode
            scanf("%c", &nextReq);
            scanf(" %[^\n]", inp);
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF); 
        }
        //Accept next Request, add to list
        reqs[reqPos] = nextReq;
        reqPos++;

        //Process the request:
        switch (nextReq) {
            case 'R':
                printRequests(reqs, reqPos);
                break;
            case 'A':
                if (addRule(inp, rulePos, ruleCap, rules) == 1) {
                    ruleCount++; 
                    rulePos++;
                }
                printf("Rule count: %d\n", ruleCount);
                break;
            case 'C':
                queryRule(inp, rules, ruleCount);
                break;
            case 'D':
                deleteRule(inp, rules, ruleCount);
                break;
            case 'L':
                listRules(rules, ruleCount);
                break;
            default:
                printf("Invalid request\n");
                break;
        }
    }
    //Free memory when we are done
    for (int i = 0; i < ruleCount; i++)
    {
        for (int j = 0; j < rules[i].queryCap; j++)
        {
            free(rules[i].queries[j]);
        }
        free(rules[i].queries);
    }
    free(reqs);
    free(rules);
    close(newsockfd);
}

