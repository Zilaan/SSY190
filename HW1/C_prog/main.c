#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// The different constans used in the PID
const double Kp = 1; // Proportional gain
const double Ki = 1; // Integral gain
const double Kd = 1; // Derivative gain
const double Tf = 1; // Filter constant
const double K  = 10; // Plant gain
const double T  = 1; // Time constant
const double h  = 0.01; // Sampling time

double c1 = (-8) * Tf;
double c2 = (-2) * h + 4 * Tf;
double c0 = 2 * (h + 2 * Tf);
double ce1 = (-8) * Kd + 2 * h * h * Ki + (-8) * Kp * Tf;
double ce2 = 4 * Kd + (h * Ki + (-2) * Kp) * (h + (-2) * Tf);
double ce0 = 4 * Kd +(h * Ki + 2 * Kp) * (h + 2 * Tf);

double plant(double a, double u, double y);
double pid(double ek, double ek_1, double ek_2, double uk_1, double uk_2);

int main(void) {
	const double a = exp(h/T);
	double y = 0; // Initial state
	double r; // Reference value
	double uk, uk_1 = 0, uk_2 = 0; // Prevous control inputs
	double ek, ek_1 = 0, ek_2 = 0; // Prevous error
	char buffer[20];

	FILE *fp; // Pointer to file

	fp = fopen("setpointvalues.txt", "r"); // Open the file in read-only

	// Check if file exists
	if(fp == NULL) {
		printf("Something went wrong");
		perror("Cannot open the file.\n");
		exit(EXIT_FAILURE);
	}

    while ( fgets(buffer, 20, fp) != NULL) {
		r = atof(buffer);
		printf("%lf\n", y);
		ek = r - y; // Error

		uk = pid(ek, ek_1, ek_2, uk_1, uk_2); // Calculate control signal
		y = plant(a, uk, y); // Update plant

		// Update signals
		uk_2 = uk_1;
		uk_1 = uk;
		ek_2 = ek_1;
		ek_1 = ek;
	}

	fclose(fp); // Close the file
	return 0;
}

double plant(double a, double u, double y) {
	return ( 1/a * (y + K * (a - 1) * u) );
}

double pid(double ek, double ek_1, double ek_2, double uk_1, double uk_2) {
	return ( (1/c0) * ((-c1 * uk_1 - c2 * uk_2) + (ce0 * ek + ce1 * ek_1 + ce2 * ek_2)) );

}
