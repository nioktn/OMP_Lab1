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

	double *parall_mass, *cons_mass;

	int i, j, n, m, threads, chunk, count1, count2, el;

	setlocale(LC_ALL, "Rus");

	printf("Enter rows number: ");
	scanf_s("%d", &n);
	printf("Enter cols number: ");
	scanf_s("%d", &m);
	printf("Enter element number in row: ");
	scanf_s("%d", &el);

	//ofstream os("C:\\lab14_100.csv", ios::app);
	//saveCSVthreads("C:\\lab14_100.csv");
	//os << "\n\n";

	// ==========
	chunk = n * 0.1;
	for (threads = 4; threads <= 44; threads += 8)
	{
		parall_mass = (double*)malloc(n * m * sizeof(double));
		ReadArray(parall_mass, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");

		count1 = 0;		
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
		printf("Parallel execution time for %d threads = %f\n", threads, time);

		//os << time << ";";

		free(parall_mass);
	}
	// ==========

	
	// ----------
	count2 = 0;
	cons_mass = (double*)malloc(n * m * sizeof(double));
	ReadArray(cons_mass, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");

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

	printf("Elements quantity: %d\n", count2);
	double time = (cons_finish.LowPart - cons_start.LowPart) * 1000.0f / cons_frequency.LowPart;
	printf("Consistent execution time = %f\n", time);
	free(cons_mass);
	// ----------

	//os << "\n";
	//os.close();
}