#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(void) {
	// The different constans used in the PID
	const double Kp = 1; // Proportional gain
	const double Ki = 1; // Integral gain
	const double Kd = 1; // Derivative gain
	const double Tf = 1; // Filter constant
	const double K  = 1; // Plant gain
	const double T  = 1; // Time constant
	const double h  = 1; // Sampling time

	FILE *fp; // Pointer to file

	fp = fopen("setpointvalues.txt", "r"); // Open the file in read-only

	// Check if file exists
	if(fp == NULL) {
		perror("Cannot open the file.\n");
		exit(EXIT_FAILURE);
	}
	fclose(fp); // Close the file
	return 0;
}
