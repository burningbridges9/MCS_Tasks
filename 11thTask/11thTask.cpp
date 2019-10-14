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
void ProcessInitialization(double* &a, double* &b,	double* &c, double* &diag,
	int &size, int &sizeGet, int &ProcNum, int &ProcRank)
{
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&sizeGet, 1, MPI_INT, 0, MPI_COMM_WORLD);
	
	b = new double[sizeGet*size];
	c = new double[sizeGet*size];
	diag = new double[sizeGet*size];
	for (int i = 0; i<sizeGet; i++)
	{
		for (int j = 0; j<size; j++)
		{
			//if (j <= i)
			b[i*size + j] = 0;
			c[i*size + j] = 0;
			diag[i*size + j] = 0;
			/*else
			Matrix[i*Size + j] = 0;*/
		}
	}
	if (ProcRank == 0)
	{
		a = new double[size*size];
		RandomDataInitialization(a, size);
	}
}
int main()
{
	int ProcRank, ProcNum;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	double* a;
	double* b;
	double* c;
	double * diag;
	int size = 8; // Sizes of initial matrix 8x8
	int sizeGet = 8; // even?odd matrix size 4x8

	//MPI_Datatype MPI_CONTIGUOUS;
	MPI_Datatype MPI_EVEN_ODD_ROWS;
	MPI_Datatype MPI_DIAG;
	//MPI_Type_contiguous(size, MPI_DOUBLE, &MPI_CONTIGUOUS);
	//MPI_Type_commit(&MPI_CONTIGUOUS);
	//MPI_Type_vector(4, 1, 2, MPI_CONTIGUOUS, &MPI_EVEN_ODD_ROWS);

	MPI_Type_vector(4, size,size*2, MPI_DOUBLE, &MPI_EVEN_ODD_ROWS);
	MPI_Type_vector(1, 1, size+1, MPI_DOUBLE, &MPI_DIAG);
	MPI_Type_commit(&MPI_EVEN_ODD_ROWS);
	MPI_Type_commit(&MPI_DIAG);
	ProcessInitialization(a, b, c, diag, size, sizeGet, ProcNum, ProcRank);

	if (ProcRank == 0)
	{
		MPI_Send(a, 1, MPI_EVEN_ODD_ROWS,
			1, 0, MPI_COMM_WORLD);
		MPI_Send(a+8, 1, MPI_EVEN_ODD_ROWS, 1, 0, MPI_COMM_WORLD);
		MPI_Send(a, 1, MPI_DIAG,
			1, 0, MPI_COMM_WORLD);
	}
	else
	{
		MPI_Recv(b, 1, MPI_EVEN_ODD_ROWS, 0,
			0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(c, 1, MPI_EVEN_ODD_ROWS, 0,
			0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(diag, 1, MPI_DIAG, 0,
			0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("\nB Matrix: \n");
		for (int i = 0; i<sizeGet; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", b[i*size + j]);
			}
			printf("\n");
		}
		printf("\nC Matrix: \n");
		for (int i = 0; i<sizeGet; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", c[i*size + j]);
			}
			printf("\n");
		}
		printf("\nDiag Matrix: \n");
		for (int i = 0; i < sizeGet; i++)
		{
			for (int j = 0; j < size; j++)
			{
				printf("%0.f  ", diag[i*size + j]);
			}
			printf("\n");
		}
		//MPI_Recv(c, 1, MPI_EVEN_ODD_ROWS, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	MPI_Finalize();
    return 0;
}

