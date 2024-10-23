#include <stdio.h>
#include <string.h>
#include <pthread.h>

int r1 = 0, r2 = 0;

void do_one_thing()
{
	int i;

	for (i = 0; i < 10; i++) {
		printf("[1] doing one thing %d\n", i); //Does one thing 10 times
	}
}

void do_another_thing()
{
	int i;

	for (i = 0; i < 10; i++) {
		printf("[2] doing another %d\n", i); //Does something else
	}
}

//Both local variables will be independent and specific to thread stack


int main(void)
{
	pthread_t thread1,  thread2; //Create threads for both funcs

	pthread_create(&thread1,
		NULL,
		(void *)do_one_thing,
		(void *)&r1);

	pthread_create(&thread2,
		NULL,
		(void *)do_another_thing,
		(void *)&r2);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	printf("[0] bye, bye\n");
	//Since we don't wait for each thread, we will get unexpected behaviour.

	return 0;
}