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

//void saveCSVthreads(const char *file_path)
//{
//	ofstream os(file_path, ios::app);
//	if (!os.is_open())
//		return;
//	for (int i = 5; i <= 50; i += 5)
//		os << i << ";";
//	os << "\n";
//	os.close();
//}

int main()
{
	LARGE_INTEGER frequency, start_p, finish_p;

	double *parall_mass1;
	char *parall_mass2;
	double *cons_mass1;
	char *cons_mass2;

	//double sum = 0;
	int i, j, n, m, threads, chunk, min, max;

	setlocale(LC_ALL, "Rus");

	n = 5000, m = 5000;
	printf("Enter rows number: 5000");
	//scanf_s("%d", &n);
	printf("\nEnter cols number: 5000");
	//scanf_s("%d", &m);
	printf("\nEnter max element: ");
	scanf_s("%d", &min);
	printf("Enter min element: ");
	scanf_s("%d", &max);

	//ofstream os("C:\\lab13_100.csv", ios::app);
	//saveCSVthreads("C:\\lab13_100.csv");
	//os << "\n\n";

	for (threads = 5; threads <= 50; threads += 5) {
		parall_mass1 = (double*)malloc(n * m * sizeof(double));
		ReadArray(parall_mass1, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");

		int count1 = 0;
		chunk = n * 0.1;
		parall_mass2 = (char*)calloc(n * m, sizeof(char));

		QueryPerformanceFrequency(&frequency);
		QueryPerformanceCounter(&start_p);

#pragma omp parallel for private(i,j) reduction(+:count1) schedule(dynamic,chunk) num_threads(threads)
		for (i = 0; i < n; i++)
		{
			for (j = 0; j < m; j++)
			{
				if (parall_mass1[i*m + j] >= min && parall_mass1[i*m + j] <= max)
				{
					parall_mass2[i*m + j] = (char)parall_mass1[i*m + j];
					count1++;
				}
				else
				{
					parall_mass2[i*m + j] = RAND_MAX + 1;
				}
			}
		}

		QueryPerformanceCounter(&finish_p);
		j = 0;
		for (i = 0; i < n*m; i++)
		{
			if (parall_mass2[i] != RAND_MAX + 1)
			{
				parall_mass2[j] = parall_mass2[i];
				j++;
			}
		}
		parall_mass2 = (char*)realloc(parall_mass2, count1 * sizeof(char));
		printf("Elements quantity: %d\n", count1);

		double time = (finish_p.LowPart - start_p.LowPart) * 1000.0f / frequency.LowPart;
		printf("Parallel execution time for %d threads = %f\n", threads, time);

		//os << time << ";";

		free(parall_mass1);
		free(parall_mass2);
	}

	int count2 = 0;
	cons_mass1 = (double*)malloc(n * m * sizeof(double));
	ReadArray(cons_mass1, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");
	cons_mass2 = (char*)calloc(n * m, sizeof(char));

	// -----------
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			if (cons_mass1[i*m + j] >= min && cons_mass1[i*m + j] <= max)
			{
				cons_mass2[i*m + j] = (char)cons_mass1[i*m + j];
				count2++;
			}
			else
			{
				cons_mass2[i*m + j] = RAND_MAX + 1;
			}
		}
	}

	QueryPerformanceCounter(&finish_p);
	j = 0;
	for (i = 0; i < n*m; i++)
	{
		if (cons_mass2[i] != RAND_MAX + 1)
		{
			cons_mass2[j] = cons_mass2[i];
			j++;
		}
	}
	cons_mass2 = (char*)realloc(cons_mass2, count2 * sizeof(char));
	printf("Elements quantity: %d\n", count2);

	double time = (finish_p.LowPart - start_p.LowPart) * 1000.0f / frequency.LowPart;
	printf("Consistent execution time = %f\n", time);
	free(cons_mass1);
	free(cons_mass2);
	// ------------

	//os << "\n";
	//os.close();
}