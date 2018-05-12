// SortMerger.cpp: определ€ет точку входа дл€ консольного приложени€.
//

#include "stdafx.h"

void ShellSort(double *, int *, int, int);					//	функци€ сортировки вставками
void ShellSort(double *, int *, int, int, int *);			//	функци€ сортировки вставками (дл€ параллельной версии)
void Merge(double[], int, double[], int, double[], int);	//	функци€ сли€ни€ массивов

#define Parallel											//	если не закомментирован массив сортируетс€ параллельно

int main()
{
	//SetConsoleCP(1251);
	//SetConsoleOutputCP(1251);
	srand(time(NULL));
	FILE *file;
#pragma warning(disable: 4996)
	
#ifdef Parallel
	const int size = 500;
	int merge = 0;
	int omp_threads = 8;											//	количество потоков в системе
	double*	massiv = new double[omp_threads*size];					//	указатель на область пам€ти в которой будет хранитьс€ исходный массив
	int Delta[7] = { 301, 132, 57, 23, 10, 4, 1 };					//	дл€ алгоритма сортировки
	
	
	double* result = new double[size*omp_threads];
	double* massiv1 = new double[size*omp_threads];
	double* massiv2 = new double[size*omp_threads];
	double* sort_massiv = new double[omp_threads*size];				//	выдел€ем пам€ть под промежуточный массив
	
	for (int i=0;i<omp_threads*size;i++)
	{
		*(massiv + i) = rand() % 32768;
	}
	for (int i=0;i<omp_threads;i++)
	{
		memcpy(sort_massiv+ i*size, massiv + i * size, sizeof(double)*(size));
	}

#pragma omp parallel for
	for (int i=0;i<omp_threads;i++)
	{
		printf("omp_get_max_threads: %d\n", omp_threads);			//	вывод в консоль количество потоков
		ShellSort(sort_massiv + omp_get_thread_num()*size, Delta, size, 7, &merge);
	}
	while (merge == omp_threads)
	{
		int i = 0;
		
		while (i<omp_threads-1)
		{
			Merge(massiv1 + i*size, size*2, (sort_massiv + i*size), size, (sort_massiv + (i + 1)*size), size);
			i += 2;
		}
		i = 0;
		while (i<omp_threads/2-1)
		{
			Merge(massiv2 + i*size*2, size * 4, (massiv1 + i*size*2), size*2, (massiv1 + (i + 1)*size*2), size*2);
			i += 2;
		}
		Merge(result, size * omp_threads, (massiv2), size * omp_threads / 2, (massiv2+size*omp_threads/2), size * omp_threads / 2);
		break;
	}
	int sort_errors = 0;
	for (int i = 0; i<(sizeof(result) / sizeof(double)); i++)
	{
		if ((result[i]>result[i + 1]) && (i + 1)<size)
		{
			printf("element #%d: %.0f\n", i, result[i]);
			printf("element #%d: %.0f\n", i + 1, result[i + 1]);
			sort_errors++;
		}

	}
	if (sort_errors>0)
	{
		printf("count errors: %d\n", sort_errors);
	}
	else
	{
		printf("success\n");
	}
	delete[] massiv;
	delete[] massiv1;
	delete[] massiv2;
	delete[] sort_massiv;
	delete[] result;
#else
	double	massiv[5000];
	//printf("size: %d\n", sizeof(massiv) / sizeof(double));
	int Delta[5] = { 57 ,23 ,10 ,4 ,1 };
	int size = (sizeof(massiv) / sizeof(double));
	for (int i = 0; i<size; i++)
	{
		massiv[i] = rand() % 30000;
	}
	ShellSort(massiv,Delta, size,5);
	int sort_errors = 0;
	for (int i = 0; i<(sizeof(massiv) / sizeof(double)); i++)
	{
		if ((massiv[i]>massiv[i+1]) && (i+1)<size)
		{
			printf("element #%d: %.0f\n", i,massiv[i]);
			printf("element #%d: %.0f\n", i+1, massiv[i+1]);
			sort_errors++;
		}
		
	}
	if (sort_errors>0)
	{
		printf("count errors: %d\n", sort_errors);
	}
	else
	{
		printf("success\n");
	}
#endif // !Parallel
	system("pause");
    return 0;
}


void ShellSort(double *arrayPtr, int *arrayDelta, int N, int N1, int *merge) // сортировка вставками дл€ параллельного алгоритма
{
	int i, j, k;
	double t;
#pragma omp parallel for private (k)
	for (int y = 0; y<N1; y++)
	{
		for (k = *(arrayDelta + y); k > 0; k /= 2)
		{
			for (i = k; i < N; i++)
			{
				t = arrayPtr[i];
				for (j = i; j >= k; j -= k)
				{
					if (t < arrayPtr[j - k])
						arrayPtr[j] = arrayPtr[j - k];
					else
						break;
				}
				arrayPtr[j] = t;
			}
		}
	}
	*merge += 1;
}

void ShellSort(double *arrayPtr, int *arrayDelta, int N, int N1) // сортировка вставками
{
	int i, j, k;
	double t;
	for (int y = 0; y<N1; y++)
	{
		for (k = *(arrayDelta + y); k > 0; k /= 2)
		{
			for (i = k; i < N; i++)
			{
				t = arrayPtr[i];
				for (j = i; j >= k; j -= k)
				{
					if (t < arrayPtr[j - k])
						arrayPtr[j] = arrayPtr[j - k];
					else
						break;
				}
				arrayPtr[j] = t;
			}
		}
	}
}

void Merge(double merged[], int lenD, double L[], int lenL, double R[], int lenR)
{
	int i = 0;
	int j = 0;
	while (i<lenL || j<lenR) {
		if (i<lenL & j<lenR) {
			if (L[i] <= R[j]) {
				merged[i + j] = L[i];
				i++;
			}
			else {
				merged[i + j] = R[j];
				j++;
			}
		}
		else if (i<lenL) {
			merged[i + j] = L[i];
			i++;
		}
		else if (j<lenR) {
			merged[i + j] = R[j];
			j++;
		}
	}
}
