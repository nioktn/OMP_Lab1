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

double ExecuteParallel(int n, int m, int min, int max, int threads, double *parall_mass1, char *parall_mass2, double *pulled_elems)
{
	LARGE_INTEGER parall_frequency, parall_start, parall_finish;

	int i, j, chunk, counter1 = 0;
	
	int count1 = 0;
	chunk = n * 0.1;
	parall_mass2 = (char*)calloc(n * m, sizeof(char));

	QueryPerformanceFrequency(&parall_frequency);
	QueryPerformanceCounter(&parall_start);

#pragma omp parallel for private(i,j) reduction(+:count1) schedule(dynamic,chunk) num_threads(threads)
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			if (parall_mass1[i*m + j] >= min && parall_mass1[i*m + j] <= max)
			{
				//parall_mass2[i*m + j] = (char)parall_mass1[i*m + j];
				parall_mass2[i*m + j] = 1;
				count1++;
			}
			//else
			//{
			//	parall_mass2[i*m + j] = RAND_MAX + 1;
			//}
		}
	}

	QueryPerformanceCounter(&parall_finish);

	j = 0;

	printf("%d", count1);

	for (i = 6543453; i < 6543553; i++)
	{
		printf("%f", parall_mass1[i]);
	}

	pulled_elems = (double*)malloc(count1 * sizeof(double));

	int elems_count = 0;
	for (i = 0; i < n; i++)
	{
		for (j = 0; j < m; j++)
		{
			if (parall_mass2[i*m + j] = 1)
			{
				pulled_elems[elems_count] = parall_mass1[i*m + j];
				elems_count++;
			}
		}
	}

	printf("%d %f %f %f", elems_count, pulled_elems[3], pulled_elems[5], pulled_elems[8]);

	printf("Elements quantity for %dx%d matrix = %d\n", n, m, count1);
	double time = (parall_finish.LowPart - parall_start.LowPart) * 1000.0f / parall_frequency.LowPart;
	printf("Parallel execution time for %d threads = %f\n", threads, time);
	return time;
}

double ExecuteConsistent(int n, int m, int min, int max, double *cons_mass1, char *cons_mass2, double *pulled_elems)
{
	LARGE_INTEGER cons_frequency, cons_start, cons_finish;
	int i, j, count2 = 0;

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

	QueryPerformanceCounter(&cons_finish);
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

	printf("Elements quantity %d", count2);
	double time = (cons_finish.LowPart - cons_start.LowPart) * 1000.0f / cons_frequency.LowPart;
	printf("Consistent execution time for %dx%d matrix = %f\n", n, m, time);
	return time;
}

int main()
{
	
	LARGE_INTEGER frequency, start_p, finish_p;

	double *cons_mass1, *parall_mass1, *pulled_elems;
	char *parall_mass2, *cons_mass2;

	double sum = 0;
	int i, j, n, m, threads, chunk, min, max;

	double *b = (double*)malloc(n*m * sizeof(double));
	ReadArray(b, n*m, "C:\\Data\\Stuff\\OpenMP\\bin\\10000.bin");
	printf("%f %f %f %f %f %f %f %f \n\n\n", b[65454], b[8454], b[5454], b[654], b[454], b[9999994], b[54], b[6453]);

	double sum1, sum2;
	int n, m, threads, q = 0;

	printf("Multiplier: ");
	scanf_s("%d", &q);

	ofstream os("C:\\Data\\Stuff\\OpenMP\\CSVs\\task11.csv", ios::app);

	printf("Enter min element: ");
	scanf_s("%d", &min);
	printf("\nEnter max element: ");
	scanf_s("%d", &max);

	string path;
	int switchvar = 0;

	os << 4 << ";" << 12 << ";" << 20 << ";" << 28 << ";" << 36 << ";" << 44 << ";" << 1 << "\n";
	while (switchvar <= 6) {
		switch (switchvar)
		{
			//case(0): path = "C:\\Data\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
			//	break;
			//case(1): path = "C:\\Data\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
			//	break;
			//case(2): path = "C:\\Data\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
			//	break;
			//case(3): path = "C:\\Data\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
			//	break;
			//case(4): path = "C:\\Data\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
			//	break;
			//case(5): path = "C:\\Data\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
			//	break;
			//case(6): path = "C:\\Data\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
			//	break;
		case(0): path = "D:\\Stuff\\OpenMP\\bin\\100.bin", n = 100, m = 100;
			break;
		case(1): path = "D:\\Stuff\\OpenMP\\bin\\1000.bin", n = 1000, m = 1000;
			break;
		case(2): path = "D:\\Stuff\\OpenMP\\bin\\2000.bin", n = 2000, m = 2000;
			break;
		case(3): path = "D:\\Stuff\\OpenMP\\bin\\4000.bin", n = 4000, m = 4000;
			break;
		case(4): path = "D:\\Stuff\\OpenMP\\bin\\6000.bin", n = 6000, m = 6000;
			break;
		case(5): path = "D:\\Stuff\\OpenMP\\bin\\8000.bin", n = 8000, m = 8000;
			break;
		case(6): path = "D:\\Stuff\\OpenMP\\bin\\10000.bin", n = 10000, m = 10000;
			break;
		}

		//Execute parallel

	ofstream os("C:\\lab13_100.csv", ios::app);
	//saveCSVthreads("C:\\lab13_100.csv");
	os << "\n\n";
	for (threads = 4; threads <= 44; threads += 8) {
		parall_mass1 = (double*)malloc(n * m * sizeof(double));
		ReadArray(parall_mass1, n*m, "‪C:\\Data\\Stuff\\OpenMP\\bin\\10000.bin");		
		parall_mass2 = (char*)calloc(n * m, sizeof(char));
		double time = ExecuteParallel(n, m, min, max, threads, parall_mass1, parall_mass2, pulled_elems);
		free(parall_mass1);
		free(parall_mass2);
	}

	int count2 = 0;
	cons_mass1 = (double*)malloc(n * m * sizeof(double));
	ReadArray(cons_mass1, n*m, "C:\\Users\\nniki\\source\\repos\\Univ\\5000.bin");
	cons_mass2 = (char*)calloc(n * m, sizeof(char));

	free(cons_mass1);
	free(cons_mass2);
	 //------------

	os << "\n";
	os.close();
}