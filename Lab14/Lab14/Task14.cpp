#include "pch.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

void ReadArray(double* pdata, size_t nm, string file_path)
{
	ifstream is(file_path, ios::binary | ios::in);
	if (!is.is_open())
		return;
	is.read((char*)(pdata), nm * sizeof(double));
	is.close();
}

void ExecuteParallel(int n, int m, int el, int threads, double *parall_mass)
{
	LARGE_INTEGER parall_frequency, parall_start, parall_finish;

	int i, j, chunk, count1 = 0;
	chunk = n * 0.1;
	QueryPerformanceFrequency(&parall_frequency);
	QueryPerformanceCounter(&parall_start);

#pragma omp parallel for private(i,j) reduction(+:count1) schedule(dynamic, chunk) num_threads(threads)
	for (i = 0; i < n; i++)
	{
		if (parall_mass[i*m + el] == 0)
		{
			count1++;
		}
	}

	QueryPerformanceCounter(&parall_finish);

	printf("Elements quantity: %d\n", count1);

	double time = (parall_finish.LowPart - parall_start.LowPart) * 1000.0f / parall_frequency.LowPart;
	printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time);
}

void ExecuteConsistent(int n, int m, int el, double *cons_mass)
{
	LARGE_INTEGER cons_frequency, cons_start, cons_finish;
	int i, count2 = 0;

	QueryPerformanceFrequency(&cons_frequency);
	QueryPerformanceCounter(&cons_start);
	for (i = 0; i < n; i++)
	{
		if (cons_mass[i*m + el] == 0)
		{
			count2++;
		}
	}
	QueryPerformanceCounter(&cons_finish);

	printf("Counter value: %d\n", count2);
	double time2 = (cons_finish.LowPart - cons_start.LowPart) * 1000.0f / cons_frequency.LowPart;
	printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time2);
}

int main()
{
	double *parall_mass, *cons_mass;
	int n, m, threads, el;

	printf("Enter element number in row: ");
	scanf_s("%d", &el);

	string path;
	int switchvar = 0;
	while (switchvar <= 6) {
		switch (switchvar)
		{
		case(0): path = "D:\\Stuff\\OpenMP\\minus\\100.bin", n = 100, m = 100;
			break;
		case(1): path = "D:\\Stuff\\OpenMP\\minus\\1000.bin", n = 1000, m = 1000;
			break;
		case(2): path = "D:\\Stuff\\OpenMP\\minus\\2000.bin", n = 2000, m = 2000;
			break;
		case(3): path = "D:\\Stuff\\OpenMP\\minus\\4000.bin", n = 4000, m = 4000;
			break;
		case(4): path = "D:\\Stuff\\OpenMP\\minus\\6000.bin", n = 6000, m = 6000;
			break;
		case(5): path = "D:\\Stuff\\OpenMP\\minus\\8000.bin", n = 8000, m = 8000;
			break;
		case(6): path = "D:\\Stuff\\OpenMP\\minus\\10000.bin", n = 10000, m = 10000;
			break;
		}

		//Execute parallel
		for (threads = 4; threads <= 44; threads += 8)
		{
			parall_mass = (double*)malloc(n * m * sizeof(double));
			ReadArray(parall_mass, n*m, path);
			ExecuteParallel(n, m, el, threads, parall_mass);
			//os << time << ";";
			free(parall_mass);
		}

		//Execute consistent
		cons_mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(cons_mass, n*m, path);
		ExecuteConsistent(n, m, el, cons_mass);
		free(cons_mass);
		//os << "\n";
		//os.close();
		switchvar++;
	}
}