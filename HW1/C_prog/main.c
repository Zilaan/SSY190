#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

// The different constans used in the PID
const double Kp = 4;		// Proportional gain
const double Ki = 0.5;		// Integral gain
const double Kd = 2;		// Derivative gain
const double Tf = 4;		// Filter constant
const double K = 2;		// Plant gain
const double T = 3;		// Time constant
const double h = 0.1;		// Sampling time

double plant(double u, double y);
double pid(double ek, double ek_1, double ek_2, double uk_1, double uk_2);

int main(void)
{
	double y = 0;		// Initial state
	double r;		// Reference value
	double uk, uk_1 = 0, uk_2 = 0;	// Prevous control inputs
	double ek, ek_1 = 0, ek_2 = 0;	// Prevous error
	char buffer[20];	// Buffer to store value from input file

	FILE *fpIn, *fpOut;	// Pointer to input and output file

	fpIn = fopen("setpointvalues.txt", "r");	// Open the input file in read-only
	fpOut = fopen("output.txt", "w");	// Open the output file in write-only

	// Check if input file exists
	if (fpIn == NULL) {
		perror("Cannot open the file.\n");
		exit(EXIT_FAILURE);
	}

	while (fgets(buffer, 20, fpIn) != NULL) {
		r = atof(buffer);	// Convert string to double
		printf("%lf\n", y);
		ek = r - y;	// Calculate the error

		uk = pid(ek, ek_1, ek_2, uk_1, uk_2);	// Calculate control signal
		y = plant(uk, y);	// Update plant
		fprintf(fpOut, "%lf\n", y);	// Write output value to file

		// Update signals
		uk_2 = uk_1;
		uk_1 = uk;
		ek_2 = ek_1;
		ek_1 = ek;
	}

	fclose(fpIn);		// Close the input file
	fclose(fpOut);		// Close the output file
	return 0;
}

double plant(double u, double y)
{
	double a = exp(h / T);	// Constant used in the plant
	return (1 / a * (y + K * (a - 1) * u));
}

double pid(double ek, double ek_1, double ek_2, double uk_1, double uk_2)
{
	/**
	* Numerator and denominator coefficients used
	* to describe the PID transfer function
	**/
	double c1 = (-8) * Tf;
	double c2 = (-2) * h + 4 * Tf;
	double c0 = 2 * (h + 2 * Tf);
	double ce1 = (-8) * Kd + 2 * h * h * Ki + (-8) * Kp * Tf;
	double ce2 = 4 * Kd + (h * Ki + (-2) * Kp) * (h + (-2) * Tf);
	double ce0 = 4 * Kd + (h * Ki + 2 * Kp) * (h + 2 * Tf);

	return ((1 / c0) *
		((-c1 * uk_1 - c2 * uk_2) +
		 (ce0 * ek + ce1 * ek_1 + ce2 * ek_2)));
}
