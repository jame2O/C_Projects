#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>


#define MAX_PORT 65535
#define MAX_IP 255
#define BUFFERLENGTH 256
#define MAX_RULE_CAP 10
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;

typedef struct Rule {
    char ip[2][16]; //2 spaces for IPs, 16 chars for each IP
    int ports[2]; // 2 spaces for ports
    char** queries;
    int queryCount;
    int queryCap;
} Rule;
// Declare all the initial stuff
int ruleCount = 0, reqPos = 0, reqCap = 10;

char *reqs;
Rule *rules;
int rulePos = 0;
int ruleCap = MAX_RULE_CAP;
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
    if (atoi(port) <= 0 || atoi(port) > MAX_PORT) return -1;
    return 1;
}

char* printRequests() {
    pthread_mutex_lock(&mut);
    // Allocate initial memory for the result string
    size_t resultSize = reqPos * 2 + 1; // Each request will add 2 characters (comma and request) + null terminator
    char* result = (char*)malloc(resultSize);
    if (!result) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    result[0] = '\0'; // Initialize the result string
    //Loop through the request array, printing each one
    for (int i=0; i<reqPos; i++) {
        char next[2];
        next[0] = reqs[i];
        next[1] = '\0';
        strcat(result, next);
        strcat(result, "\n");
    }
    pthread_mutex_unlock(&mut);
    return result;
}

char *addRule(char* inp)
{
    pthread_mutex_lock(&mut);
    char *result = malloc(50 * sizeof(char));
    if (!result)
    {
        pthread_mutex_unlock(&mut);
        return NULL;
    }

    // Checks if we need to add more memory to the rules list.
    if (rulePos >= ruleCap)
    {
        ruleCap *= 2;
        Rule *newRules = (Rule *)realloc(rules, ruleCap * sizeof(Rule));
        if (!newRules)
        {
            strcpy(result, "Memory allocation failed\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        rules = newRules;
    }

    // Split IP & Port & check validation
    char ips[50], ports[50];
    if (sscanf(inp, "%s %s", ips, ports) != 2)
    {
        strcpy(result, "Invalid rule\n");
        pthread_mutex_unlock(&mut);
        return result;
    }

    // Check the IP and split if necessary, then verify, and add to the rules list
    if (strchr(ips, '-'))
    { // Multiple case
        char ip1[20], ip2[20];
        sscanf(ips, "%[^-]-%s", ip1, ip2);
        if (validateIp(ip1) == -1 || validateIp(ip2) == -1)
        {
            strcpy(result, "Invalid rule\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        else
        {
            if (compareIps(ip1, ip2) == -1)
            {
                strcpy(result, "Invalid rule\n");
                pthread_mutex_unlock(&mut);
                return result;
            }
            else
            { // Add to rules list
                strcpy(rules[rulePos].ip[0], ip1);
                strcpy(rules[rulePos].ip[1], ip2);
            }
        }
    }
    else
    { // Single case
        if (validateIp(ips) == -1)
        {
            strcpy(result, "Invalid rule\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        else
        {
            strcpy(rules[rulePos].ip[0], ips);
            strcpy(rules[rulePos].ip[1], "#");
        }
    }

    // Check the port and split if necessary, then verify, and add to the rules list
    if (strchr(ports, '-'))
    { // Multiple case
        char port1[10], port2[20];
        sscanf(ports, "%[^-]-%s", port1, port2);
        if (validatePort(port1) == -1 || validatePort(port2) == -1 || atoi(port2) <= atoi(port1))
        {
            strcpy(result, "Invalid rule\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        else
        {
            rules[rulePos].ports[0] = atoi(port1);
            rules[rulePos].ports[1] = atoi(port2);
        }
    }
    else
    { // Single case
        if (validatePort(ports) == -1)
        {
            strcpy(result, "Invalid rule\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        else
        {
            rules[rulePos].ports[0] = atoi(ports);
            rules[rulePos].ports[1] = -1;
        }
    }

    rules[rulePos].queryCount = 0;
    rules[rulePos].queryCap = 10; // Initial capacity for queries
    rules[rulePos].queries = (char **)malloc(rules[rulePos].queryCap * sizeof(char *));
    if (!rules[rulePos].queries)
    {
        strcpy(result, "Memory allocation failed\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    for (int i = 0; i < rules[rulePos].queryCap; i++)
    {
        rules[rulePos].queries[i] = (char *)malloc(30 * sizeof(char)); // Allocate space for each query
        if (!rules[rulePos].queries[i])
        {
            for (int j = 0; j < i; j++)
            {
                free(rules[rulePos].queries[j]);
            }
            free(rules[rulePos].queries);
            strcpy(result, "Memory allocation failed\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
    }

    // Print response
    strcpy(result, "Rule added\n");
    pthread_mutex_unlock(&mut);
    return result;
}

char* queryRule(char inp[]) {
    pthread_mutex_lock(&mut);
    char* result = malloc(100*sizeof(char));
    // Split IP & Port & check validation
    char ip[50], port[50];
    if (sscanf(inp, "%s %s", ip, port) != 2)
    {
        strcpy(result, "Illegal IP address or port specified\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    // Check the IP 
    if (validateIp(ip) == -1)
    {
        strcpy(result, "Illegal IP address or port specified\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    // Check the port
    if (validatePort(port) == -1)
    {
        strcpy(result, "Illegal IP address or port specified\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    //Checks against rules
    for (int i=0; i<ruleCount; i++) {
        //Check IP First
        if ((rules[i].ip[1][0] == '#')) { //Single IP case
            if (!(strcmp(rules[i].ip[0], ip) == 0))
            {
                continue;
            }
        } else { //Multiple IP case
            if (!(compareIps(rules[i].ip[0], ip) == 1 && compareIps(inp, rules[i].ip[1]) == 1))
            {
                continue;
            }
        }
        //Now check ports
        if (rules[i].ports[1] != -1) { //Multiple port case
            if (!(atoi(port) >= rules[i].ports[0] && atoi(port) <= rules[i].ports[1])) {
                continue;
            }
        } else { //Single port case
            if (!(atoi(port) == rules[i].ports[0])) {
                continue;
            }
        }
        //All checks passed, add to queries
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
        strcpy(result, "Connection accepted\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    strcpy(result, "Connection rejected\n");
    pthread_mutex_unlock(&mut);
    return result;
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
char* deleteRule(char inp[]) {
    pthread_mutex_lock(&mut);
    char* result = malloc(50*sizeof(char));
    // Split IP & Port & check validation
    char ips[50], ports[50];
    if (sscanf(inp, "%s %s", ips, ports) != 2)
    {
        strcpy(result, "Rule invalid\n");
        pthread_mutex_unlock(&mut);
        return result;
    }
    if (strchr(ips, '-'))
    { // Multiple case
        char ip1[20], ip2[20];
        sscanf(ips, "%[^-]-%s", ip1, ip2);
        if (validateIp(ip1) == -1 || validateIp(ip2) == -1)
        {
            strcpy(result, "Rule invalid\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
        else
        {
            if (compareIps(ip1, ip2) == -1)
            {
                strcpy(result, "Rule invalid\n");
                pthread_mutex_unlock(&mut);
                return result;
            }
        }
    }
    else
    { // Single case
        if (validateIp(ips) == -1)
        {
            strcpy(result, "Rule invalid\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
    }
    if (strchr(ports, '-'))
    { // Multiple case
        char port1[10], port2[20];
        sscanf(ports, "%[^-]-%s", port1, port2);
        if (validatePort(port1) == -1 || validatePort(port2) == -1 || atoi(port2) <= atoi(port1))
        {
            strcpy(result, "Rule invalid\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
    }
    else
    { // Single case
        if (validatePort(ports) == -1)
        {
            strcpy(result, "Rule invalid\n");
            pthread_mutex_unlock(&mut);
            return result;
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
            strcpy(result, "Rule deleted\n");
            pthread_mutex_unlock(&mut);
            return result;
        }
    }
    strcpy(result, "Rule invalid\n");
    pthread_mutex_unlock(&mut);
    return result;
}
char* listRules() {
    size_t resultSize = 1024; // Initial size for the result string
    char* result = (char*)malloc(resultSize);
    if (!result) {
        fprintf(stderr, "Memory allocation failed!\n");
        exit(1);
    }
    result[0] = '\0'; // Initialize the result string

    for (int i = 0; i < ruleCount; i++) {
        char ruleString[70];
        rejoinRule(rules[i], ruleString);

        // Calculate the required size and reallocate if necessary
        size_t requiredSize = strlen(result) + strlen("Rule: ") + strlen(ruleString) + 2;
        if (requiredSize > resultSize) {
            resultSize = requiredSize + 1024;
            result = (char*)realloc(result, resultSize);
            if (!result) {
                fprintf(stderr, "Memory reallocation failed!\n");
                exit(1);
            }
        }

        strcat(result, "Rule: ");
        strcat(result, ruleString);
        strcat(result, "\n");

        for (int j = 0; j < (rules[i].queryCount); j++) {
            if (rules[i].queries[j][0] != '\0') {
                // Calculate the required size and reallocate if necessary
                requiredSize = strlen(result) + strlen("Query: ") + strlen(rules[i].queries[j]) + 2;
                if (requiredSize > resultSize) {
                    resultSize = requiredSize + 1024;
                    result = (char*)realloc(result, resultSize);
                    if (!result) {
                        fprintf(stderr, "Memory reallocation failed!\n");
                        exit(1);
                    }
                }

                strcat(result, "Query: ");
                strcat(result, rules[i].queries[j]);
                strcat(result, "\n");
            }
        }
    }
    pthread_mutex_unlock(&mut);
    return result;
}
void interactiveMode() {
    // Interactive mode
    // Declare the requests list and allocate mem for 10
    while (1) {
        char nextReq;
        char inp[50]; 
        if (scanf(" %c", &nextReq) == EOF) {
            break;
        }
        if (nextReq != 'R' && nextReq != 'L') {
            if ((scanf(" %[^\n]", inp)) == EOF) {
                break;
            }
        }

        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);
        // Checks if we need to add more memory to the request list.
        if (reqPos >= reqCap)
        {
            reqCap *= 2;
            reqs = (char *)realloc(reqs, reqCap * sizeof(char));
        }
        //Accept next Request, add to list
        reqs[reqPos] = nextReq;
        reqPos++;

        //Process the request:
        switch (nextReq) {
            case 'R':
                char* resultR = printRequests();
                printf("%s", resultR);
                free(resultR);
                break;
            case 'A':
                char* resultA = addRule(inp);
                if (strcmp(resultA, "Rule added\n") == 0) {
                    ruleCount++;
                    rulePos++;
                    printf("%s", resultA);
                    free(resultA);
                } else {
                    printf("%s", resultA);
                    free(resultA);
                }
                break;
            case 'C':
                char* resultC = queryRule(inp);
                printf("%s", resultC);
                free(resultC);
                break;
            case 'D':
                char* resultD = deleteRule(inp);
                if (strcmp(resultD, "Rule deleted\n") == 0) {
                    ruleCount--;
                    rulePos--;
                }
                printf("%s", resultD);
                free(resultD);
                break;
            case 'L':
                char* resultL = listRules(1, rules, ruleCount);
                printf("%s", resultL);
                free(resultL);
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
    exit(1);
}
int writeResult (int sockfd, char *buffer, size_t bufsize) {
    int n;
   
    n = write(sockfd, &bufsize, sizeof(size_t));
    if (n < 0) {
		fprintf (stderr, "ERROR writing to result\n");
		return -1;
    }
    
    n = write(sockfd, buffer, bufsize);
    if (n != bufsize) {
		fprintf (stderr, "Couldn't write %ld bytes, wrote %d bytes\n", bufsize, n);
		return -1;
    }
    return 0;
}
char *readRes(int sockfd) {
    size_t bufsize;
    int res;
    char *buffer;

    res = read(sockfd, &bufsize, sizeof(size_t));
    if (res != sizeof(size_t)) {
		fprintf (stderr, "Reading number of bytes from socket failed\n");
		return NULL;
    }

    buffer = malloc(bufsize+1);
    if (buffer) {
		buffer[bufsize]  = '\0';
		res = read(sockfd, buffer, bufsize);
		if (res != bufsize) {
			fprintf (stderr, "Reading reply from socket\n");
			free(buffer);
			return NULL;
		}
    }
    
    return buffer;
}  

void *processRequest (void *args) {
    int *newsockfd =  (int *) args;   
    char *buffer;
    buffer = readRes (*newsockfd);
    if (!buffer)  {
		fprintf (stderr, "ERROR reading from socket\n");
    }
    else {
        char inp = buffer[0];
        char args[100] = "";
        if (inp != 'R' && inp != 'L')
        {
            sscanf(buffer+1, " %[^\n]", args);
        }
        printf ("Here is the request: %s\n", buffer);
        // Checks if we need to add more memory to the request list.
        if (reqPos >= reqCap)
        {
            reqCap *= 2;
            reqs = (char *)realloc(reqs, reqCap * sizeof(char));
        }
        // Accept next Request, add to list
        reqs[reqPos] = inp;
        reqPos++;
        // Process the request:
        switch (inp) {
        case 'R':
            char *resultR = printRequests();
            writeResult(*newsockfd, resultR, strlen(resultR) + 1);
            free(resultR);
            break;
        case 'A':
            char *resultA = addRule(args);
            if (strcmp(resultA, "Rule added\n") == 0)
            {
                ruleCount++;
                rulePos++;
                writeResult(*newsockfd, resultA, strlen(resultA) + 1);
                free(resultA);
            }
            else
            {
                writeResult(*newsockfd, resultA, strlen(resultA) + 1);
                free(resultA);
            }
            break;
        case 'C':
            char *resultC = queryRule(args);
            writeResult(*newsockfd, resultC, strlen(resultC) + 1);
            free(resultC);
            break;
        case 'D':
            char *resultD = deleteRule(args);
            if (strcmp(resultD, "Rule deleted\n") == 0)
            {
                ruleCount--;
                rulePos--;
            }
            writeResult(*newsockfd, resultD, strlen(resultD) + 1);
            free(resultD);
            break;
        case 'L':
            char *resultL = listRules(1, rules, ruleCount);
            writeResult(*newsockfd, resultL, strlen(resultL) + 1);
            free(resultL);
            break;
        default:
            char* invalidResponse = "Invalid request\n";
            writeResult(*newsockfd, invalidResponse, strlen(invalidResponse) + 1);
            break;
        }
    }

    free(buffer);
    close(*newsockfd);
    free(newsockfd); 
    pthread_exit (NULL); /*exit value not used */
}
// Main Loop for the server program
int main (int argc, char ** argv) {
    //Allocate initial memory for the request and rule lists
    reqs = (char*)malloc(reqCap*sizeof(char));
    rules = (Rule*)malloc(ruleCap*sizeof(Rule));
    //Check for interactive mode first as to not waste memory
    int result;
    socklen_t clilen;
    int sockfd, newsockfd, portno;
    struct sockaddr_in6 serv_addr;
    if (argc >= 2) {
        if (strcmp(argv[1], "-i") == 0) {
            interactiveMode();
        } else {
            if (atoi(argv[1]) > 0 && atoi(argv[1]) <= MAX_PORT ) {
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
                
            } else {
                printf("Invalid argument\n");
                return 0;
            }
        }
    } else {
        printf("Invalid argument\n");
        return 0;
    }
    
    
    // Run the program indefinetly
    while (1) {
        // Thread stuff
        pthread_t server_thread;
        pthread_attr_t pthread_attr;
        int *newsockfd;
        struct sockaddr_in6 cli_addr;
        clilen = sizeof (cli_addr);
        newsockfd = malloc(sizeof (int));
        if (!newsockfd) { //NEW SOCK ALERRTT
            fprintf (stderr, "Memory allocation failed!\n");
            exit(1);
        }
        /* waiting for connections */
		*newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (*newsockfd < 0) {
			error ("ERROR on accept");
		}
        
        // Create new thread for connection processing
        if (pthread_attr_init (&pthread_attr)) {
            fprintf(stderr, "ERROR on thread initial attrs creation");
            exit(1);
        }
        if (pthread_attr_setdetachstate (&pthread_attr, PTHREAD_CREATE_DETACHED)) {
            fprintf(stderr, "ERROR on thread attrs setting");
            exit(1);
        }
        result = pthread_create (&server_thread, &pthread_attr, processRequest, (void *) newsockfd);
        if (result != 0) {
			fprintf (stderr, "Thread creation failed!\n");
			exit (1);
		}
    }
    close(newsockfd);

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
    free(rules);/* important to avoid memory leak */
    return 0;
}

