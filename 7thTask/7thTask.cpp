// 7thTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;

void DataInit(double* &x, double* &xPart, int ProcNum, int ProcRank, int &size, int &chunk)
{
	chunk = size / ProcNum;
	//send this to define size of local array
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
	x = new double[size];
	xPart = new double[chunk];
	// подготовка данных
	if (ProcRank == 0)
	{
		srand(unsigned(clock()));
		printf("\nVector: \n");
		for (int i = 0; i<size; i++)
		{
			x[i] = rand() / double(1000);
			printf("%f  ", x[i]);
		}
		printf("\n");
	}
}

void DataDistribution(double* &x, double* &xPart, int chunk, int ProcRank)
{
	MPI_Scatter(x + chunk*ProcRank, chunk, MPI_DOUBLE, 
				xPart, chunk, MPI_DOUBLE, 
				0, MPI_COMM_WORLD);

	printf("\nChunked vector on process %d: \n", ProcRank);
	for (int i = 0; i<chunk; i++)
	{
		printf("%f  ", xPart[i]);
	}
	printf("\n");
}


void Calculs(double* &xPart, int chunk, int ProcRank, double & localSum, double &globalSum)
{
	for (int i = 0; i < chunk; i++)
		localSum += xPart[i];
	//printf("\nlocSum on process %d: %f\n", ProcRank, localSum);
	MPI_Reduce(&localSum, &globalSum, 1, 
		MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
}

int main()
{
	int ProcRank, ProcNum, chunk;
	double * x;
	double * xPart;
	double localSum = 0;
	double globalSum=0;
	MPI_Status st;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int size = 2 * ProcNum;
	//распределение данных по процессам
	DataInit(x, xPart, ProcNum, ProcRank, size, chunk);
	DataDistribution(x, xPart, chunk, ProcRank);
	Calculs(xPart, chunk, ProcRank, localSum, globalSum);
	
	//check
	if (ProcRank==0)
	{
		printf("\nParallel sum result: %f\n", globalSum);
		double nonParallelSum = 0;
		for (int i = 0; i < size; i++)
			nonParallelSum += x[i];
		printf("\nNon-parallel sum result: %f\n", globalSum);
	}
	MPI_Finalize();
    return 0;
}

