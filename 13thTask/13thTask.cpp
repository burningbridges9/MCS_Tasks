// 13thTask.cpp: определяет точку входа для консольного приложения.
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
void RandomDataInitialization(double* Matrix, int size)
{
	srand(unsigned(clock()));
	printf("\nA Matrix on Rank 0: \n");
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			if (j >= i)
				Matrix[i*size + j] = rand() / double(1000);
			else
				Matrix[i*size + j] = 0;
			printf("%0.f  ", Matrix[i*size + j]);
		}
		printf("\n");
	}
}

// Function for memory allocation and data initialization
void ProcessInitialization(double* &a,
	int &size, int &ProcNum, int &ProcRank)
{
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);	
	a = new double[size*size];
	printf("\nA Matrix BEFORE: \n");
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			a[i*size + j] = 0;
			printf("%0.f  ", a[i*size + j]);
		}
		printf("\n");
	}
	if (ProcRank == 0)
	{
		
		RandomDataInitialization(a, size);
	}
}
int main()
{
	int ProcRank, ProcNum;
	double* a;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int size = 8; // Sizes of initial matrix 8x8
	ProcessInitialization(a, size,  ProcNum, ProcRank);
	int * blockLengths = new int[size];
	int * indices = new int[size];
	for (int i = 0; i < size; i++) 
	{
		blockLengths[i] = size - i;
		indices[i] = i * size + i;
	}
	MPI_Datatype MPI_UPPER;
	MPI_Type_indexed(size, blockLengths, indices, MPI_DOUBLE, &MPI_UPPER);
	MPI_Type_commit(&MPI_UPPER);
	MPI_Bcast(a, 1, MPI_UPPER, 0, MPI_COMM_WORLD);
	if (ProcRank != 0)
	{
		printf("\nA Matrix AFTER: \n");
		for (int i = 0; i<size; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", a[i*size + j]);
			}
			printf("\n");
		}
	}
	MPI_Finalize();
	return 0;
}

