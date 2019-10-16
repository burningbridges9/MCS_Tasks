// 12thTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;
// Function for random definition of matrix and vector elements
void RandomDataInitialization(int* a, double* b,  int sizeA, int sizeB)
{
	srand(unsigned(clock()));
	for (int i = 0; i < sizeA; i++)
	{
		a[i] = rand() / double(1000);
		if (i < sizeB)
		{
			b[i] = rand() / double(1000);
		}
	}

	printf("\nvector a on 0 proc : \n");
	for (int i = 0 ; i<sizeA; i++)
		printf("%d  ", a[i]);
	printf("\nvector b on 0 proc : \n");
	for (int i = 0; i<sizeB; i++)
		printf("%0.f  ", b[i]);
	printf("\n");
}

// Function for memory allocation and data initialization
void ProcessInitialization(int* &a, double* &b, double* &buf,
	int &sizeA, int &sizeB, int &ProcNum, int &ProcRank)
{
	MPI_Bcast(&sizeA, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&sizeB, 1, MPI_INT, 0, MPI_COMM_WORLD);
	buf = new double[sizeA+sizeB];
	b = new double[sizeB];
	a = new int[sizeA];
	for (int i = 0; i < sizeA; i++)
	{
		a[i] = 0;
		if (i < sizeB)
		{
			b[i] = 0;
		}
	}
	if (ProcRank == 0)
	{
		RandomDataInitialization(a, b, sizeA, sizeB);
	}
}
int main()
{
	int ProcRank, ProcNum, chunkRows;	
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int* a;
	double* b;
	double * buf;
	int pos = 0;
	int sizeA = 10;
	int sizeB = 8; 
	int k1 = sizeA * sizeof(int);
	int k2 = sizeB * sizeof(double);
	int k = (sizeA+sizeB) * sizeof(double);
	ProcessInitialization(a, b, buf, sizeA, sizeB, ProcNum, ProcRank);
	MPI_Pack(a, sizeA, MPI_INT, buf, 144, &pos, MPI_COMM_WORLD);
	MPI_Pack(b, sizeB, MPI_DOUBLE, buf, 144, &pos, MPI_COMM_WORLD);
	MPI_Bcast(buf, 144, MPI_PACKED, 0, MPI_COMM_WORLD);
	if (ProcRank != 0)
	{
		pos = 0;
		MPI_Unpack(buf, 144, &pos, a, sizeA, MPI_INT, MPI_COMM_WORLD);
		MPI_Unpack(buf, 144, &pos, b, sizeB, MPI_DOUBLE, MPI_COMM_WORLD);

		printf("\n UNPACKED vector a on 0 proc : \n", ProcRank);
		for (int i = 0; i<sizeA; i++)
			printf("%d  ", a[i]);
		printf("\n UNPACKED vector b on 0 proc : \n", ProcRank);
		for (int i = 0; i<sizeB; i++)
			printf("%0.f  ", b[i]);
		printf("\n");
	}
	MPI_Finalize();
	return 0;
}

