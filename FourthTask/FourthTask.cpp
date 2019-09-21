// FourthTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
using namespace std;


void DataInit(double* &globalArrX, double* &globalArrY, double* &globalArrZ, 
	double* &localArrX, double* &localArrY, double* &localArrZ, 
	int ProcNum, int ProcRank, int &N, int &chunk)
{
	//send this to define size of local array
	MPI_Bcast(&N, 1, MPI_INT, 0, MPI_COMM_WORLD);

	int k = N / ProcNum;
	int i1 = k * ProcRank;
	int i2 = k * (ProcRank + 1);
	if (ProcRank == ProcNum - 1) 
		i2 = N;
	chunk = i2 - i1;
	localArrX = new double[chunk];
	globalArrX = new double[N];
	localArrY = new double[chunk];
	globalArrY = new double[N];
	localArrZ = new double[chunk];
	globalArrZ = new double[N];
	// подготовка данных
	if (ProcRank == 0)
	{
		printf("\nCreated global array X:\n");
		for (int i = 0; i < N; i++)
		{
			globalArrX[i] = rand() % 5;
			printf("%.0f ", globalArrX[i]);
		}
		printf("\nCreated global array Y:\n");
		for (int i = 0; i < N; i++)
		{
			globalArrY[i] = rand() % 10;
			printf("%.0f ", globalArrY[i]);
		}
	}
}

void SendRecv()
{
	int ProcRank, ProcNum, chunk;
	double * globalArrX;
	double * localArrX;
	double * globalArrY;
	double * localArrY;
	double * globalArrZ;
	double * localArrZ;
	MPI_Status st;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int N = 10*ProcNum;
	//распределение данных по процессам
	DataInit(globalArrX, globalArrY,  globalArrZ,
			localArrX, localArrY, localArrZ, 
			ProcNum, ProcRank, N, chunk);

	if (ProcRank == 0) 
	{
		//in proc0 copy globarr to localarr
		for (int i = 0; i < chunk; i++)
		{
			localArrX[i] = globalArrX[i];
			localArrY[i] = globalArrY[i];
			globalArrZ[i] = localArrX[i] + localArrY[i];
		}
		//send to other processes' local arrays x,y from global arrays x, y
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Send(globalArrX + i*chunk, chunk, MPI_DOUBLE, 
				i, 0, MPI_COMM_WORLD);
			MPI_Send(globalArrY + i*chunk, chunk, MPI_DOUBLE, 
				i, 0, MPI_COMM_WORLD);
		}
	}
	else
	{
		MPI_Recv(localArrX, chunk, MPI_DOUBLE, 0,
			0, MPI_COMM_WORLD, &st);

		
		MPI_Recv(localArrY, chunk, MPI_DOUBLE, 0,
			0, MPI_COMM_WORLD, &st);	

		
		// make calculs
		for (int i = 0; i < chunk; i++)
		{
			localArrZ[i] = localArrX[i] + localArrY[i]; 
			//localArrZ[i] = localArrX[i] * localArrY[i]; 
		}
		/*printf("\nrecieving parts of locarrx to rank %i:\n", ProcRank);
		for (int j = 0; j < chunk; j++)
		{
			printf("%.0f ", localArrX[j]);
		}
		printf("\nrecieving parts of locarry to rank %i:\n", ProcRank);
		for (int j = 0; j < chunk; j++)
		{
			printf("%.0f ", localArrY[j]);
		}
		printf("\nrecieving parts of locarrz to rank %i:\n", ProcRank);
		for (int j = 0; j < chunk; j++)
		{
			printf("%.0f ", localArrZ[j]);
		}*/
	}
	


	// сбор будет на 0 процессе
	if (ProcRank == 0) 
	{
		for (int i = 1; i < ProcNum; i++)
		{
			MPI_Recv(localArrZ, chunk, MPI_DOUBLE, i,
				MPI_ANY_TAG, MPI_COMM_WORLD, &st);
			/*printf("\nrecieving parts of locarrz to rank %i:\n", ProcRank);
			for (int j = 0; j < chunk; j++)
			{
				printf("%.0f ", localArrZ[j]);
			}*/
			printf("\n");
			for (int j = 0; j < chunk; j++)
			{
				globalArrZ[i*chunk + j] = localArrZ[j];
			}
		}
	}
	else
	{
		/*printf("\nparts of locarrz to rank %i:\n", ProcRank);
		for (int j = 0; j < chunk; j++)
		{
			printf("%.0f ", localArrZ[j]);
		}*/
		MPI_Send(localArrZ, chunk, MPI_DOUBLE,
			0, 0, MPI_COMM_WORLD);
	}

	if (ProcRank == 0)
	{
		printf("\nResult array into rank %i:\n", ProcRank);
		for (int j = 0; j < N; j++)
		{
			printf("%.0f ", globalArrZ[j]);
		}
	}

	MPI_Finalize();

}

int main()
{
	SendRecv();
    return 0;
}

