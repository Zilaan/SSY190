#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

sem_t s; // Declaration of Semaphore

void *thread1(void *arg) {
	struct timespec sleepValue;
	sleepValue.tv_sec = 1;
	sleepValue.tv_nsec = 0;

	while (1) {
		sem_wait(&s);

		/* critical section */
		printf("Thread 1 in critical section\n");
		printf("Argument 1: %s\n", (char*)arg);
		/* critical section */

		sem_post(&s);
		nanosleep(&sleepValue, NULL);
	}
	return NULL;
}

void *thread2(void *arg) {
	struct timespec sleepValue;
	sleepValue.tv_sec = 2;
	sleepValue.tv_nsec = 0;

	while (1) {
		sem_wait(&s);

		/* critical section */
		printf("Thread 2 in critical section\n");
		printf("Argument 2: %s\n", (char*)arg);
		/* critical section */

		sem_post(&s);
		nanosleep(&sleepValue, NULL);
	}
	return NULL;
}

int main() {
	pthread_t threadArray[2];	// array of thread IDs
	int status;		// error code
	pthread_attr_t threadAttribute;	// thread attribute

	// initialize semaphore s
	// First argument is the address to the semaphore
	// Second argument is if it is shared between threads in the same process (set to 0)
	// or between separate processes (set to 1).
	// Third argument is the initial value of the semaphore
	sem_init(&s, 0, 1);

	// initialize the thread attribute object
	status = pthread_attr_init(&threadAttribute);
	pthread_attr_setscope(&threadAttribute, PTHREAD_SCOPE_SYSTEM);

	// Create two threads and store their IDs in array threadArray

	char thread1Arg[] = "Hello 1";
	pthread_create(&threadArray[0], &threadAttribute, thread1, (void *)&thread1Arg);

	char thread2Arg[] = "Hello 2";
	pthread_create(&threadArray[1], &threadAttribute, thread2, (void *)&thread2Arg);

	status = pthread_attr_destroy(&threadAttribute);	// destroy the attribute object

	// Wait for threads to finish
	status = pthread_join(threadArray[0], NULL);
	status = pthread_join(threadArray[1], NULL);

	// Destroy semaphore s
	sem_destroy(&s);
}
