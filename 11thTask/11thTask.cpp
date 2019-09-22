// 11thTask.cpp: определяет точку входа для консольного приложения.
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
	printf("\nMatrix: \n");
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			//if (j <= i)
			Matrix[i*size + j] = rand() / double(1000);
			printf("%0.f  ", Matrix[i*size + j]);
			/*else
			Matrix[i*Size + j] = 0;*/
		}
		printf("\n");
	}
}

// Function for memory allocation and data initialization
void ProcessInitialization(double* &a, double* &b,	double* &c, 
	int &size, int &sizeGet, int &ProcNum, int &ProcRank)
{
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&sizeGet, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	b = new double[sizeGet*size];
	c = new double[sizeGet*size];
	if (ProcRank == 0)
	{
		a = new double[size*size];
		RandomDataInitialization(a, size);
	}
}
int main()
{
	int ProcRank, ProcNum, chunkRows;
	double* a; 
	double* b;
	double * c;
	double* Result;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int size = 8; // Sizes of initial matrix 8x8
	int sizeGet = 4; // even?odd matrix size 4x8

	MPI_Datatype MPI_EVEN_ODD_ROWS;
	MPI_Type_vector(sizeGet, size, 2*size, MPI_DOUBLE, &MPI_EVEN_ODD_ROWS);
	MPI_Type_commit(&MPI_EVEN_ODD_ROWS);

	ProcessInitialization(a, b, c, size, sizeGet, ProcNum, ProcRank);

	if (ProcRank == 0)
	{
		MPI_Send(a, 1, MPI_EVEN_ODD_ROWS, 1, 0, MPI_COMM_WORLD);
		//MPI_Send(a+8, 1, MPI_EVEN_ODD_ROWS, 1, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(b, 1, MPI_EVEN_ODD_ROWS, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("\nB Matrix: \n");
		for (int i = 0; i<sizeGet; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", b[i*sizeGet + j]);
			}
			printf("\n");
		}
		//MPI_Recv(c, 1, MPI_EVEN_ODD_ROWS, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	MPI_Finalize();
    return 0;
}

