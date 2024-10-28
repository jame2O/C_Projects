#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct list_t {
    char *s;
    struct list_t *next;
};

int main(int argc, char **argv) 
{
    char *s1 = "This is a ";
    char *s2 = "simple string test";
    
    char *s;

    // List ops
    struct list_t *node0, *node1;

    node0 = malloc(sizeof(struct list_t));
    if (node0 == NULL) {
      fprintf (stderr, "Cannot allocate memory, exiting.\n");
      exit(1);
    }
    node0->s = s1;

    printf("node0 at %p, next = %p\n", node0, node0->next);
    
    node1 = malloc(sizeof(struct list_t));
    if (node1 == NULL) {
      fprintf (stderr, "Cannot allocate memory, exiting.\n");
      free(node0);
      exit(1);
    }

    node1->s = s2;
    node1->next = NULL;
    
    // Complete appending node 1 to node 0
    node0->next = node1;

    printf("node0 at %p, next = %p\n", node0, node0->next);
    printf("node1 at %p, next = %p\n", node1, node1->next);


    // String ops
    s = malloc(strlen(s1) + strlen(s2) + 1);
    if (s == NULL) {
      fprintf (stderr, "Cannot allocate memory, exiting.\n");
      free(node0);
      free(node1);
      exit(1);
    }

    strcpy(s, s1);
    strcat(s, s2);

    printf("%s\n", s);
    free(node1);
    free(node0);
    free(s);
    return 0;
}
    
    

    
    
	
