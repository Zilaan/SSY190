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

// Controller coefficients
double c1;
double c2;
double c0;
double ce1;
double ce2;
double ce0;

double a;	// Constant used in the plant;

double y;	// Most recent plant output;
double u;	// Most recent control output

// Delayed controller outputs and errors
double uk_1;	
double uk_2;
double ek;
double ek_1;
double ek_2;
double r;

int referenceSize;	// Number of setpoints

int i;	// Variable which makes sure that the plant thread ends

char buffer[20];

FILE *fpIn, *fpOut;	// Pointers to input and output file

void *controller(void *arg)
{
	time_t sec = 1;
	long int msec = 0; // Sleep time in microseconds
	

	while (fgets(buffer, 20, fpIn) != NULL) {
		
		
		sem_wait(&s);	// Wait for the plant to finish

		// Calculation of control signal

		r = atof(buffer);	// Read the latest setpoint

		ek = r - y;	// Calculate the error
		
		// Calculation of new control signal
		u = ((1.0f / c0) * ((-c1 * uk_1 - c2 * uk_2) +
					(ce0 * ek + ce1 * ek_1 + ce2 * ek_2)));

		printf("Controller: %f\n", u);

		uk_2 = uk_1;
		uk_1 = u;
		ek_2 = ek_1;
		ek_1 = ek;

		
		sem_post(&s);
		sem_post(&s_init);
		nanosleep((struct timespec[]){{sec, msec}}, NULL);	// Have the controller sleep so that the plant can run 10 consecutive times
	}

	return NULL;
}

void *plant(void *arg)
{
	time_t sec = 0;
	long int msec = 100*NANO_SECOND_MULTIPLIER; // Sleep time in microseconds
	i = referenceSize*10;
	while (i) {
	
		sem_wait(&s_init);
		sem_wait(&s);
		i--;
		
		y = (1.0f / a * (y + K * (a - 1.0f) * u));
		
		printf("Plant: %f\n", y);
		
		fprintf(fpOut, "%lf\n", y);

		sem_post(&s_init);
		sem_post(&s);
		nanosleep((struct timespec[]){{sec, msec}}, NULL);	// Sleep for 100 milliseconds
	}
	return NULL;
}

int main()
{
	pthread_t threadArray[2];	// array of thread IDs
	int status;		// error code
	pthread_attr_t threadAttribute;	// thread attribute

	// Initialize all the variables and coefficients

	y = 0.1;	// Initial value of y

	a = exp(hp/T);
	c1 = (-8.0f) * Tf;
	c2 = (-2.0f) * h + 4.0f * Tf;
	c0 = 2.0f * (h + 2.0f * Tf);
	ce1 = (-8.0f) * Kd + 2.0f * h * h * Ki + (-8.0f) * Kp * Tf;
	ce2 = 4.0f * Kd + (h * Ki + (-2.0f) * Kp) * (h + (-2.0f) * Tf);
	ce0 = 4.0f * Kd + (h * Ki + 2.0f * Kp) * (h + 2.0f * Tf);

	// Count the number of setpoint entrys in the .txt file

	fpIn = fopen("setpointvalues.txt", "r");	// Open the input file in read-only
	
	while(fgets(buffer, 20, fpIn) != NULL) {
		referenceSize++;
	}
	
	fclose(fpIn);
	printf("Number of reference points: %d\n", referenceSize);
	
	// Open the setpoint file again, and create an output file
	
	fpIn = fopen("setpointvalues.txt", "r");
	fpOut = fopen("output.txt", "w");	// Open the output file in write-only
	
	
	
	// Initialize semaphore s
	// First argument is the address to the semaphore
	// Second argument is if it is shared between threads in the same process (set to 0)
	// Or between separate processes (set to 1).
	// Third argument is the initial value of the semaphore
	sem_init(&s, 0, 1);	// Semaphore to keep the threads from manipulating the shared variables at the same time
	sem_init(&s_init, 0, 0);	// Semaphore which makes sure that the controller is executed first, when the program begins

	// Initialize the thread attribute object
	status = pthread_attr_init(&threadAttribute);
	pthread_attr_setscope(&threadAttribute, PTHREAD_SCOPE_SYSTEM);

	// Create two threads and store their IDs in array threadArray
	
	//char thread2Arg[] = "plant";
	pthread_create(0, &threadAttribute, plant, (void *)&thread2Arg);

	//char thread1Arg[] = "controller";
	pthread_create(0, &threadAttribute, controller, (void *)&thread1Arg);
	
	status = pthread_attr_destroy(&threadAttribute);	// Destroy the attribute object

	// Wait for threads to finish
	status = pthread_join(threadArray[0], NULL);
	status = pthread_join(threadArray[1], NULL);

	// Destroy semaphore s
	sem_destroy(&s);
	sem_destroy(&s_init);

	// Close both files
	fclose(fpIn);
	fclose(fpOut);

}
