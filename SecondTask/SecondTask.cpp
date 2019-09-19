// SecondTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
using namespace std;


int main()
{
	int size, rank;
	MPI_Status st;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int  N = 10;
	double * x = new double[N];
	/*int dest = rank + 1;
	int source = rank - 1;
	if (rank == 0)
	{
		source = MPI_PROC_NULL;
	}
	if (rank == size - 1)
	{
		dest = MPI_PROC_NULL;
	}*/

	// подготовка данных
	if (rank == 0)
	{
		printf("Array values on 0 process:\n");
		for (int i = 0; i < N; i++)
		{
			x[i] = rand() % 100;
			printf("x[%d] = %f ", i, x[i]);
		}
		MPI_Send(x, N, MPI_DOUBLE,
			1, 1, MPI_COMM_WORLD);
	}
	if (rank == 1)
	{
		MPI_Recv(x, N, MPI_DOUBLE,
			0, 1, MPI_COMM_WORLD, &st);	
		printf("Array values on process %d:\n", rank);
		for (int i = 0; i < N; i++)
		{
			printf("x[%d] = %f ", i, x[i]);
		}
	}
	delete x;
	MPI_Finalize();
	return 0;
}
