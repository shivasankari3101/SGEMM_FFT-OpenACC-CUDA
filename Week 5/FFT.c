/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   Shady Boukhary
*   Midwestern State University
*   Research Week 4 - Fast Fourier Transform: Cooley Tukey
*   March 15th, 2018
*
*
*   Computes a radix-2 fast fourier transform using an iterative implementation of
*	the Cooley-Tukey Algorithm. This code uses OpenACC to parallelize loops
*
*	To compile: pgcc -acc -Minfo -o main  FFT_OpenACC.c
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#include <stdio.h>
#include <stdlib.h>      
#include <complex.h>    
#include <math.h>
#include "timer.h"

#define PI 3.14159265
#define SIZE 1000

double _Complex * computeFFT(double _Complex *, int);

double _Complex eIThetta(int, int, int, int);


void printFFT(const double _Complex *, int);

int main()
{
	double start, stop, elapsed;

	double _Complex * signals = (double _Complex*)malloc(SIZE * sizeof(double _Complex));
	double _Complex * fft = (double _Complex*)malloc(SIZE * sizeof(double _Complex));

	for (int x = 0; x < SIZE; x++) {
		signals[x] = x + (SIZE - x) * I;
	}
	// double _Complex comp =  3.6 + 2.6 * I; 
	// double _Complex comp1 = 2.9 + 6.3 * I;
	// double _Complex comp2 = 5.6 + 4.0 * I;
	// double _Complex// make sure all threads computed current phase
	// double _Complex comp4 = 3.3 + 0.4 * I;
	// double _Complex comp5 = 5.9 + 4.8 * I;
	// double _Complex comp6 = 5.0 + 2.6 * I;
	// double _Complex comp7 = 4.3 + 4.1 * I;

	// signals[0] = comp;;
	// signals[1] = comp1;
	// signals[2] = comp2;
	// signals[3] = comp3;
	// signals[4] = comp4;
	// signals[5] = comp5;
	// signals[6] = comp6;
	// signals[7] = comp7;


	GET_TIME(start);
	
	fft = computeFFT(signals, SIZE);
	GET_TIME(stop);
	elapsed = stop - start;
	printFFT(fft, SIZE);
	printf("Code to be timed took %e seconds.\n", elapsed);
	//getchar();

}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*   computeFFT()
*   @param: double _Complex, int
*   @return: double _Complex
*   Description: computes FFT for a signal
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
double _Complex * computeFFT(double _Complex * numbers, int N) {

	/* X: energy
		k: frequency
		n: signal
		eIThetta: spin of the signal at different angles
		
		Basically calculates the energy at specific frequencies by spinning
		the signal around a circle at that frequency and summing up various points
		around that circle															*/

	// array to hold X(k)
	double _Complex * X = (double _Complex*)malloc(N * sizeof(double _Complex));

	// copy array of complex numbers and energies array into device
	// copy the energies array back to host when done
	// declare a parallel region
	//#pragma acc region
	// indicate a parallel loop
    //#pragma acc parallel loop independent
	// compute all X(K)

	for (int k = 0; k < N; k++) {
		double _Complex sumEven = 0.0 + 0.0 * I;
		double _Complex sumOdd = 0.0 + 0.0 * I;
		//#pragma acc loop vector(1024)
		for (int n = 0; n <= (N / 2) - 1; n++) {
			// compute the even part

			double _Complex comp = numbers[2 * n];
			double _Complex eThetta = eIThetta(k, N, n, 0);
			//double _Complex resultEven = mult_Complex(comp, eThetta);
			double _Complex resultEven = comp * eThetta;
			//sumEven = add_Complex(resultEven, sumEven);
			sumEven += resultEven;

			// compute the odd part

			double _Complex compOdd = numbers[2 * n + 1];
			double _Complex eThettaOdd = eIThetta(k, N, n, 1);
			//double _Complex resultOdd = mult_Complex(compOdd, eThettaOdd);
			double _Complex resultOdd = compOdd * eThettaOdd;
			//sumOdd = add_Complex(resultOdd, sumOdd);
			sumOdd = resultOdd + sumOdd;
		}
		// compute X(k)
		X[k] = sumEven + sumOdd;
	}
	return X;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*   eIThetta()
*   @param: int, int, int, int
*   @return: double _Complex
*   Description: computes the spin of the signal around a circle at its frequency
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
double _Complex eIThetta(int k, int N, int n, int offset) {
	// compute real part
	double realPart = cos((2 * PI * (2 * n + offset) * k) / N);

	// compute imaginary part
	double imaginaryPart = (-1) * sin((2 * PI * (2 * n + offset) * k) / N);

	// create a _Complex number out of them and return it
	double _Complex result = realPart + imaginaryPart * I;
	return result;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*   printFFT()
*   @param: double _Complex, int
*   @return: none
*   Description: prints the FFT (components of the signal etc..)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void printFFT(const double _Complex * fft, int N) {
	//for (int i = 0; i < N; i++) {
		//printf("X(%i) = %f + %fi\n", i, creal(fft[i]), cimag(fft[i]));
	//}
	printf("X(1) = %f + %fi\nX(N-1) = %f + %fi\n", creal(fft[1]), cimag(fft[1]), creal(fft[N - 1]), cimag(fft[N - 1]));
}


