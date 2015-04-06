#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#define NANO_SECOND_MULTIPLIER  1000000  // 1 millisecond = 1,000,000 Nanoseconds

sem_t s; // Declaration of Semaphore
sem_t s_init;

const double Kp = 1.0f;		// Proportional gain
const double Ki = 0.4f;		// Integral gain
const double Kd = 0.5f;		// Derivative gain
const double Tf = 4.0f;		// Filter constant
const double K = 2.0f;		// Plant gain
const double T = 3.0f;		// Time constant
const double h = 1.0f;		// Controller sampling time
const double hp = 0.1f;		// Plant sampling time

double c1;
double c2;
double c0;
double ce1;
double ce2;
double ce0;

double a;	// Constant used in the plant;

double y;	//Most recent plant output;
double u;
double uk_1;
double uk_2;
double ek;
double ek_1;
double ek_2;
double r;

int referenceSize = 0;

int i = 120;

char buffer[20];

FILE *fpIn, *fpOut;	// Pointer to input and output file

//double plant(double u, double y);
//double pid(double ek, double ek_1, double ek_2, double uk_1, double uk_2);

void *controller(void *arg)
{
	time_t sec = 0;
	long int msec = 110 * NANO_SECOND_MULTIPLIER; // Sleep time in microseconds
	

	while (fgets(buffer, 20, fpIn) != NULL) {
		
		
		sem_wait(&s);

		// Calculation of control signal

		r = atof(buffer);

		ek = r - y;

		u = ((1.0f / c0) * ((-c1 * uk_1 - c2 * uk_2) +
					(ce0 * ek + ce1 * ek_1 + ce2 * ek_2)));

		//printf("Controller: %f\n", u);
		printf("Controller\n");
		//printf("Argument 1: %s\n", (char*)arg);

		uk_2 = uk_1;
		uk_1 = u;
		ek_2 = ek_1;
		ek_1 = ek;

		// Calculation of control signal

		sem_post(&s);
		sem_post(&s_init);
		nanosleep((struct timespec[]){{sec, msec}}, NULL);
	}

	return NULL;
}

void *plant(void *arg)
{
	time_t sec = 0;
	long int msec = 10 * NANO_SECOND_MULTIPLIER; // Sleep time in microseconds
	//i = referenceSize;
	while (i) {
	
		sem_wait(&s_init);
		sem_wait(&s);
		i--;
		
		y = (1.0f / a * (y + K * (a - 1.0f) * u));
		
		//printf("Plant: %f\n", u);
		printf("Plant\n");
		//printf("y: %f", y);
		
		fprintf(fpOut, "%lf\n", y);

		//sem_post(&s_init);
		sem_post(&s);
		nanosleep((struct timespec[]){{sec, msec}}, NULL);
	}
	return NULL;
}

int main()
{
	pthread_t threadArray[2];	// array of thread IDs
	int status;		// error code
	pthread_attr_t threadAttribute;	// thread attribute

	y = 0.1;	// Initial value of y

	a = exp(hp/T);

	c1 = (-8.0f) * Tf;
	c2 = (-2.0f) * h + 4.0f * Tf;
	c0 = 2.0f * (h + 2.0f * Tf);
	ce1 = (-8.0f) * Kd + 2.0f * h * h * Ki + (-8.0f) * Kp * Tf;
	ce2 = 4.0f * Kd + (h * Ki + (-2.0f) * Kp) * (h + (-2.0f) * Tf);
	ce0 = 4.0f * Kd + (h * Ki + 2.0f * Kp) * (h + 2.0f * Tf);

	fpIn = fopen("setpointvalues.txt", "r");	// Open the input file in read-only
	
	while(fgets(buffer, 20, fpIn) != NULL) {
		referenceSize++;
	}
	
	fclose(fpIn);
	
	printf("Number of reference points: %d\n", referenceSize);
	
	fpIn = fopen("setpointvalues.txt", "r");
	fpOut = fopen("output.txt", "w");	// Open the output file in write-only
	
	
	
	// initialize semaphore s
	// First argument is the address to the semaphore
	// Second argument is if it is shared between threads in the same process (set to 0)
	// or between separate processes (set to 1).
	// Third argument is the initial value of the semaphore
	sem_init(&s, 0, 1);
	sem_init(&s_init, 0, 0);

	// initialize the thread attribute object
	status = pthread_attr_init(&threadAttribute);
	pthread_attr_setscope(&threadAttribute, PTHREAD_SCOPE_SYSTEM);

	// Create two threads and store their IDs in array threadArray

	char thread1Arg[] = "Hello 1";
	pthread_create(&threadArray[0], &threadAttribute, controller, (void *)&thread1Arg);

	char thread2Arg[] = "Hello 2";
	pthread_create(&threadArray[1], &threadAttribute, plant, (void *)&thread2Arg);

	status = pthread_attr_destroy(&threadAttribute);	// destroy the attribute object

	// Wait for threads to finish
	status = pthread_join(threadArray[0], NULL);
	status = pthread_join(threadArray[1], NULL);

	// Destroy semaphore s
	sem_destroy(&s);

	fclose(fpIn);
	fclose(fpOut);

}
