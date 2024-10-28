#include <stdio.h>
#include <string.h>
#include <pthread.h>

volatile int global1 = 0;

void do_one_thing()
{
	for (int i = 0; i < 100000; i++) {
		global1++;
	}
}

void do_another_thing()
{
	for (int i = 0; i < 100000; i++) {
		global1++;
	}
}


int main(void)
{
	pthread_t thread1,  thread2;

	pthread_create(&thread1,
		NULL,
		(void *)do_one_thing,
		NULL);

	pthread_create(&thread2,
		NULL,
		(void *)do_another_thing,
		NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	//The problem is that both threads race against eachother 
	// To read and increment global1.

	printf("[0] bye, bye, global = %d\n", global1);

	return 0;
}