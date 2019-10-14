// 5thTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;

void DataInit(double* &matrix, double* &x, double* &matrixPart,
	double* &xPart, int ProcNum, int ProcRank, int &size, int &chunk)
{
	chunk = size / ProcNum;
	//send this to define size of local array
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
	matrix = new double[size*size];
	x = new double[size];
	matrixPart = new double[chunk*size];
	xPart = new double[chunk];
	// подготовка данных
	if (ProcRank == 0)
	{
		srand(unsigned(clock()));
		printf("\nMatrix: \n");
		for (int i = 0; i<size; i++)
		{
			for (int j = 0; j<size; j++)
			{
				//if (j <= i)
				matrix[i*size + j] = rand() / double(1000);
				printf("%f  ", matrix[i*size + j]);
				/*else
				Matrix[i*Size + j] = 0;*/
			}
			printf("\n");
		}
		/*printf("\nVector: \n");
		for (int i = 0; i<size; i++)
		{
			x[i] = rand() / double(1000);
			printf("%f  ", x[i]);
		}*/
		printf("\n");
	}
}

int main()
{
	int ProcRank, ProcNum, chunk;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	
	double * matrix;
	double * x;
	double * matrixLines;
	double * xPart;
	MPI_Status st;

	
	int size = 2 * ProcNum;
	//распределение данных по процессам
	DataInit(matrix, x, matrixLines,
		xPart, ProcNum, ProcRank, size, chunk);
	chunk = size / ProcNum;
	if (ProcRank == 0)
	{		
		
		//printf("\nProcess %d:\n", ProcRank);
		//printf("\nRecieving matrix lines:\n");
		for (int i = 0; i < chunk; i++)
		{
			for (int j = 0; j < size; j++)
			{
				matrixLines[i*size + j] = matrix[i*size + j];
				if (i == j)
					x[i] = matrixLines[i*size + j];
			//	printf("%f  ", matrixLines[i*size + j]);
			}
			//printf("\n");
		}
		//send to other processes' local arrays x,y from global arrays x, y
		for (int i = 1; i < ProcNum; i++)
		{			
			MPI_Send(matrix + i*chunk*size, chunk*size, MPI_DOUBLE,
				i, 0, MPI_COMM_WORLD);
			MPI_Send(x + i*chunk, chunk, MPI_DOUBLE,
				i, 0, MPI_COMM_WORLD);
		}

		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(x + i*chunk, chunk, MPI_DOUBLE,
				i, 0, MPI_COMM_WORLD, &st);
		}
		for (int i = 0; i < size; i++)
		{
			printf("%f  \n", x[i]);
		}
	}
	else
	{
				
		MPI_Recv(matrixLines, chunk*size, MPI_DOUBLE, 0,
			0, MPI_COMM_WORLD, &st);
		/*printf("\nProcess %d:\n", ProcRank);
		printf("\nRecieving matrix lines:\n");
		for (int i = 0; i < chunk; i++)
		{
			for (int j = 0; j < size; j++)
				printf("%f  ", matrixLines[i*size + j]);
			printf("\n");
		}*/

		/*MPI_Recv(xPart, chunk, MPI_DOUBLE, 0,
			0, MPI_COMM_WORLD, &st);

		printf("\nRecieving vector parts:\n");
		for (int i = 0; i < chunk; i++)
		{
			printf("%f  ", matrixLines[i]);			
		}
		printf("\n");*/
		// make calculs
		//printf("\n");
		for (int i = 0; i < chunk; i++)
		{
			int j = chunk*ProcRank + i;
			xPart[i] = matrixLines[i*size + j];
			//printf("%f  \n", xPart[i]);
		}
		MPI_Send(xPart, chunk, MPI_DOUBLE,
			0, 0, MPI_COMM_WORLD);
	}
	MPI_Finalize();
    return 0;
}

