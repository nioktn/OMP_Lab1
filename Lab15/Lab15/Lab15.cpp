#include "pch.h"

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include <windows.h>

using namespace std;

void ReadArray(double* pdata, size_t nm, const char* file_path)
{
	ifstream is(file_path, ios::binary | ios::in);
	if (!is.is_open())
		return;
	is.read((char*)(pdata), nm * sizeof(double));
	is.close();
}

int main()
{
	LARGE_INTEGER parall_frequency, parall_start, parall_finish;
	LARGE_INTEGER cons_frequency, cons_start, cons_finish;

	double *parall_mass1, *parall_mass2, *cons_mass1, *cons_mass2;
	double sum1, sum2;
	int i, j, n, m, threads, chunk, avg, count1, count2;

	setlocale(LC_ALL, "Rus");

	printf("Enter rows number: ");
	scanf_s("%d", &n);
	printf("Enter cols number: ");
	scanf_s("%d", &m);
	printf("Enter number of elements: ");
	scanf_s("%d", &avg);

	//ofstream os("C:\\lab15_100.csv", ios::app);
	//saveCSVthreads("C:\\lab15_100.csv");
	//os << "\n\n";

	// ==========
	for (threads = 4; threads <= 44; threads += 8) {
		parall_mass1 = (double*)malloc(n * m * sizeof(double));
		ReadArray(parall_mass1, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");

		count1 = 0;
		sum1 = 0;
		chunk = n * 0.1;
		parall_mass2 = (double*)calloc(ceil((n*m) / avg), sizeof(double));

		QueryPerformanceFrequency(&parall_frequency);
		QueryPerformanceCounter(&parall_start);

        #pragma omp parallel for private(i) reduction(+:count1) schedule(dynamic,chunk) num_threads(threads)
		for (i = 0; i < n*m; i++)
		{
			if (i%avg == avg - 1) {
				sum1 += parall_mass1[i];
				parall_mass2[(i + 1) / avg - 1] = sum1;
				sum1 = 0;
				count1++;
			}
			else
			{
				sum1 += parall_mass1[i];
			}
		}

		QueryPerformanceCounter(&parall_finish);

		printf("Number of average values: %d\n", count1);
		double time1 = (parall_finish.LowPart - parall_start.LowPart) * 1000.0f / parall_frequency.LowPart;
		printf("Parallel execution time for %d threads = %f\n", threads, time1);

		//os << time << ";";

		free(parall_mass1);
		free(parall_mass2);
		// ==========

		// ----------		
		cons_mass1 = (double*)malloc(n * m * sizeof(double));
		ReadArray(cons_mass1, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");

		count2 = 0;
		sum2 = 0;
		chunk = n * 0.1;
		cons_mass2 = (double*)calloc(ceil((n*m) / avg), sizeof(double));

		QueryPerformanceFrequency(&cons_frequency);
		QueryPerformanceCounter(&cons_start);

		for (i = 0; i < n*m; i++)
		{
			if (i%avg == avg - 1) {
				sum2 += cons_mass1[i];
				cons_mass2[(i + 1) / avg - 1] = sum2;
				sum2 = 0;
				count2++;
			}
			else
			{
				sum2 += cons_mass1[i];
			}
		}
		QueryPerformanceCounter(&cons_finish);

		printf("Number of average values : %d\n", count2);
		double time2 = (cons_finish.LowPart - cons_start.LowPart) * 1000.0f / cons_frequency.LowPart;
		printf("Consistent execution time = %f\n", time2);
		free(cons_mass1);
		free(cons_mass2);
		// ----------
	}
	//os << "\n";
	//os.close();
}