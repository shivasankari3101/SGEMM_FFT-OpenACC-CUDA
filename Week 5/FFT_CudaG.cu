/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
*   Shady Boukhary
*   Midwestern State University
*   Research Week 5 - CUDA - Fast Fourier Transform: Cooley Tukey
*   March 23rd, 2018
*
*
*   Computes a radix-2 fast fourier transform using an iterative implementation of
*	the Cooley-Tukey Algorithm. This code uses CUDA 
*
*	To compile: nvcc -o fft FFT_CudaG.cu
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-++-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+*/

#include <stdio.h>
#include <stdlib.h>      
#include <complex.h>
#include <cuComplex.h>    
#include <math.h>
#include "timer.h"
#include <cuda.h>

#define PI 3.14159265
#define SIZE 1000

//double _Complex * computeFFT(double _Complex *, int);

__device__ 
cuDoubleComplex eIThetta(int, int, int, int);


void printFFT(const cuDoubleComplex *, int);

__global__ void FFTKernel(cuDoubleComplex * numbers, cuDoubleComplex * X, int N) {
	int tx = threadIdx.x + blockDim.x * blockIdx.x;
	cuDoubleComplex sumEven = make_cuDoubleComplex(0, 0); 
	cuDoubleComplex sumOdd = make_cuDoubleComplex(0, 0);
	for (int n = 0; n <= (N / 2)-1; n++) {
		// compute the even part

		cuDoubleComplex comp = numbers[2 * n];
		cuDoubleComplex eThetta = eIThetta(tx, N, n, 0);
		//double _Complex resultEven = mult_Complex(comp, eThetta);
		cuDoubleComplex resultEven = cuCmul(comp, eThetta);
		//sumEven = add_Complex(resultEven, sumEven);
		sumEven = cuCadd(resultEven, sumEven);

		// compute the odd part

		cuDoubleComplex compOdd = numbers[2 * n + 1];
		// if (tx == 1024) {
        //     printf("number: %f + %f\n", cuCreal(compOdd), cuCimag(compOdd));
        // }
		cuDoubleComplex eThettaOdd = eIThetta(tx, N, n, 1);

		//double _Complex resultOdd = mult_Complex(compOdd, eThettaOdd);
		cuDoubleComplex resultOdd = cuCmul(compOdd, eThettaOdd);
		//sumOdd = add_Complex(resultOdd, sumOdd);
		sumOdd = cuCadd(resultOdd, sumOdd);
		// if (tx == 1601) {
        //     printf("sumOdd: %f + %f\n", cuCreal(sumOdd), cuCimag(sumOdd));
        // }
	}
	X[tx] = cuCadd(sumEven, sumOdd);
	//printf("X(%i) = %f + %fi\n", tx, cuCreal(X[tx]), cuCimag(X[tx]) );
}

int main()
{
	double start, stop, elapsed;

	cuDoubleComplex * signals = (cuDoubleComplex*)malloc(SIZE * sizeof(cuDoubleComplex));
    cuDoubleComplex * fft = (cuDoubleComplex*)malloc(SIZE * sizeof(cuDoubleComplex));
    cuDoubleComplex * signalsD;
    cuDoubleComplex * fftD;
	
	double size = SIZE * sizeof(cuDoubleComplex);

	for (int x = 0; x < SIZE; x++) {
		signals[x] = make_cuDoubleComplex(x, SIZE - x); //x + (SIZE - x) * I;
	}
	cudaMalloc((void **)&signalsD, size);
	cudaMalloc((void **)&fftD, size);

	cudaMemcpy(signalsD, signals, size, cudaMemcpyHostToDevice);

	dim3 dimGrid0(SIZE / 1024, 1, 1);
	dim3 dimBlock0(1024, 1, 1);
	GET_TIME(start);
	FFTKernel<<<dimGrid0, dimBlock0>>>(signalsD, fftD, SIZE);
	//fft = computeFFT(signals, SIZE);
	cudaDeviceSynchronize();
	GET_TIME(stop);
	elapsed = stop - start;

	cudaMemcpy(fft, fftD, size, cudaMemcpyDeviceToHost);
	printFFT(fft, SIZE);
	printf("Code to be timed took %e seconds.\n", elapsed);
	//getchar();
	cudaFree(signalsD);
	cudaFree(fftD);

}


/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*   eIThetta()
*   @param: int, int, int, int
*   @return: double _Complex
*   Description: computes the spin of the signal around a circle at its frequency
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
__device__
cuDoubleComplex eIThetta(int k, int N, int n, int offset) {
	// compute real part
	double realPart = cos((2 * PI * (2 * n + offset) * k) / N);

	// compute imaginary part
	double imaginaryPart = (-1) * sin((2 * PI * (2 * n + offset) * k) / N);

	// create a _Complex number out of them and return it
	cuDoubleComplex result = make_cuDoubleComplex(realPart, imaginaryPart);//realPart + imaginaryPart * I;
	return result;
}

/*+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
*   printFFT()
*   @param: double _Complex, int
*   @return: none
*   Description: prints the FFT (components of the signal etc..)
+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-*/
void printFFT(const cuDoubleComplex * fft, int N) {
	//for (int i = 0; i < N; i++) {
		//printf("X(%i) = %f + %fi\n", i, creal(fft[i]), cimag(fft[i]));
	//}
	printf("X(1) = %f + %fi\nX(N-1) = %f + %fi\n", cuCreal(fft[1]), cuCimag(fft[1]), cuCreal(fft[N - 1]), cuCimag(fft[N - 1]));
	//printf("X(1601): %f + %f\n", cuCreal(fft[1601]), cuCimag(fft[1601]));
}


