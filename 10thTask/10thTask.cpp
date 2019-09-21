// 10thTask.cpp: определяет точку входа для консольного приложения.
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
void RandomDataInitialization(double* Matrix, double* Vector, int Size) {
	srand(unsigned(clock()));
	printf("\nMatrix: \n");
	for (int i = 0; i<Size; i++)
	{
		for (int j = 0; j<Size; j++)
		{
			//if (j <= i)
			Matrix[i*Size + j] = rand() / double(1000);
			printf("%f  ", Matrix[i*Size + j]);
			/*else
			Matrix[i*Size + j] = 0;*/
		}
		printf("\n");
	}
	printf("\nVector: \n");
	for (int i = 0; i<Size; i++)
	{
		Vector[i] = rand() / double(1000);
		printf("%f  ", Vector[i]);
	}
	printf("\n");
}

// Function for memory allocation and data initialization
void ProcessInitialization(double* &Matrix, double* &Vector,
				double* &Result, double* &ProcRows, double* &ProcResult,
				int &Size, int &ProcNum, int &ProcRank , int &chunkRows)
{
	// how many rows will be in proc
	chunkRows = Size / ProcNum;
	MPI_Bcast(&Size, 1, MPI_INT, 0, MPI_COMM_WORLD); 
	MPI_Bcast(&chunkRows, 1, MPI_INT, 0, MPI_COMM_WORLD);
	Vector = new double[Size];
	Result = new double[Size];
	ProcRows = new double[chunkRows*Size];
	ProcResult = new double[chunkRows];
	if (ProcRank == 0) 
	{
		Matrix = new double[Size*Size];
		RandomDataInitialization(Matrix, Vector, Size);
	}
}

// Data distribution among the processes
void DataDistribution(double* Matrix, double* ProcRows, double* Vector,
	int Size, int RowNum, int &ProcNum, int &ProcRank) 
{	
	MPI_Bcast(Vector, Size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	// Scatter the rows
	MPI_Scatter(Matrix + Size*RowNum*ProcRank, RowNum*Size,  MPI_DOUBLE, 
		ProcRows, RowNum*Size, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
	//printf("Process %d recieved lines:\n", ProcRank);
	//for (int i = 0; i<RowNum; i++)
	//{
	//	for (int j = 0; j<Size; j++)
	//	{
	//		//if (j <= i)
	//		printf("%f  ", ProcRows[i*Size + j]);
	//		/*else
	//		Matrix[i*Size + j] = 0;*/
	//	}
	//	printf("\n");
	//}
}

// Function for calculating partial matrix-vector multiplication
void ParallelResultCalculation(double* ProcRows, double* Vector, double*
	ProcResult, int Size, int RowNum) 
{
	for (int i = 0; i<RowNum; i++)
	{
		ProcResult[i] = 0; 
		for (int j = 0; j<Size; j++)
			ProcResult[i] += ProcRows[i*Size + j] * Vector[j];
	}
}

// Function for gathering the result vector
void ResultReplication(double* ProcResult, double* Result, int Size,
	int RowNum, int &ProcNum, int &ProcRank)
{	
	//Gather the whole result vector on every processor
	MPI_Gather(ProcResult, RowNum, MPI_DOUBLE, 
		Result + RowNum*ProcRank, RowNum, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
}

int main()
{
	int ProcRank, ProcNum, chunkRows;
	double* Matrix; // The first argument - initial matrix
	double* Vector; // The second argument - initial vector
	double* Result;
	double * ProcResult;// Result vector for matrix-vector multiplication
	double* ProcRows;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int Size = 2 * ProcNum; // Sizes of initial matrix and vector
	//if (ProcRank == 0)
	ProcessInitialization(Matrix, Vector, Result, ProcRows, ProcResult,
	Size, ProcNum, ProcRank, chunkRows);
	DataDistribution(Matrix, ProcRows, Vector, Size, chunkRows, ProcNum, ProcRank);
	ParallelResultCalculation(ProcRows, Vector, ProcResult, Size, chunkRows);
	ResultReplication(ProcResult, Result, Size, chunkRows, ProcNum, ProcRank);
	if (ProcRank == 0)
	{
		printf("\nParallel Result vector:\n");
		for (int i = 0; i < Size; i++)
		{
			printf("%f  ", Result[i]);
		}
		printf("\n");
		double* ResultSeq = new double[Size];
		for (int i = 0; i<Size; i++)
		{
			ResultSeq[i] = 0;
			for (int j = 0; j<Size; j++)
				ResultSeq[i] += Matrix[i*Size + j] * Vector[j];
		}
		printf("\nNon-Parallel Result vector:\n");
		for (int i = 0; i < Size; i++)
		{
			printf("%f  ", ResultSeq[i]);
		}
		printf("\n");

	}
	//delete Matrix, Vector, Result;
	MPI_Finalize();
    return 0;
}

