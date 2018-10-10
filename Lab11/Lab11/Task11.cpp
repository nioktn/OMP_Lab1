#include "pch.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <windows.h>
#include <locale.h>

using namespace std;

void saveCSVthreads(const char *file_path)
{
	ofstream os(file_path, ios::app);
	if (!os.is_open())
		return;
	for (int i = 4;i <= 44;i += 8)
		os << i << ";";
	os << "\n";
	os.close();
}

void ReadArray(double* pdata, size_t nm, string file_path)
{
	ifstream is(file_path, ios::binary | ios::in);
	if (!is.is_open())
		return;
	is.read((char*)(pdata), nm * sizeof(double));
	is.close();
}


double ExecuteParallel(int n, int m, int q, int threads, double *parall_mass)
{
	int i, j;
	double sum;
	int chunk = n * 0.1;
	LARGE_INTEGER parall_frequency, parall_start, parall_finish;

	// =========
	sum = 0;
	QueryPerformanceFrequency(&parall_frequency);
	QueryPerformanceCounter(&parall_start);

	//#pragma omp parallel for shared(parall_mass) private(i,j) reduction(+:sum) schedule(dynamic, chunk) num_threads(threads)
#pragma omp parallel shared(parall_mass) private(i, j) num_threads(threads)
	{
#pragma omp for reduction(+:sum) schedule(dynamic, chunk) nowait
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < m; j++)
			{
				parall_mass[i * m + j] *= q;
				sum += parall_mass[i * m + j];
			}
		}
	}
	QueryPerformanceCounter(&parall_finish);
	printf("Summ: %f\n", sum);
	double time1 = (parall_finish.LowPart - parall_start.LowPart) * 1000.0f / parall_frequency.LowPart;
	printf("Parallel execution time for %dx%d matrix with %d threads = %f\n", n, m, threads, time1);
}

double ExecuteConsistent(int n, int m, int q, double *cons_mass)
{
	int i, j;
	double sum;
	LARGE_INTEGER cons_frequency, cons_start, cons_finish;

	sum = 0;
	QueryPerformanceFrequency(&cons_frequency);
	QueryPerformanceCounter(&cons_start);
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			cons_mass[i * m + j] *= q;
			sum += cons_mass[i * m + j];
		}
	}

	QueryPerformanceCounter(&cons_finish);
	printf("Summ: %f\n", sum);
	double time2 = (cons_finish.LowPart - cons_start.LowPart) * 1000.0f / cons_frequency.LowPart;
	printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time2);
	return time2;
	// ------------
}

int main()
{
	setlocale(0, "RUS");


	double *cons_mass, *parall_mass;
	double sum1, sum2;
	int n, m, threads, q = 0;

	printf("Multiplier: ");
	scanf_s("%d", &q);

	//ofstream os("C:\\Users\\nniki\\source\\repos\\Univ\\task11_5000.csv", ios::app);
	//saveCSVthreads("C:\\Users\\nniki\\source\\repos\\Univ\\task11_5000.csv");
	ofstream os("D:\\Stuff\\OpenMP\\CSVs\\task11.csv", ios::app);
	saveCSVthreads("D:\\Stuff\\OpenMP\\CSVs\\task11.csv");

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
			double time1 = ExecuteParallel(n, m, q, threads, parall_mass);
			os << time1 << ";";
			free(parall_mass);
		}

		//Execute consistent
		cons_mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(cons_mass, n*m, path);
		double time2 = ExecuteConsistent(n, m, q, cons_mass);
		os << time2 << ";";
		free(cons_mass);

		switchvar++;
	}
	os << "\n";
	os.close();
}
