// 6thTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;

void DataInit(double* &x, int ProcNum, int ProcRank, int &size)
{
	//send this to define size of local array
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	x = new double[size];
	// подготовка данных
	if (ProcRank == 0)
	{
		srand(unsigned(clock()));
		//printf("\nValue - Index: \n");
		for (int i = 0; i<size; i++)
		{
			x[i] = rand() / double(1000);
		}
		//printf("\n");
	}
}

void PingPong(double* &x, int ProcRank, int &size)
{
	int count = 0;
	int limit = 2;
	int partner = (ProcRank + 1) % 2;
	while (count < limit) {
		if (ProcRank == count % 2) {
			// Increment the ping pong count before you send it
			count++;
			MPI_Send(x, size, MPI_DOUBLE, partner, 0,
				MPI_COMM_WORLD);
			printf("%d sent and incremented ping_pong_count "
				"%d to %d\n", ProcRank, count,
				partner);
		}
		else {
			MPI_Recv(x, size, MPI_DOUBLE, partner, 0,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			printf("%d sent and incremented ping_pong_count "
				"%d to %d\n", ProcRank, count,
				partner);
		}
	}
}

int main()
{
	int ProcRank, ProcNum;
	double * x;
	MPI_Status st;
	double t1, t2, dt;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int size = 10;
	//распределение данных по процессам
	DataInit(x, ProcNum, ProcRank, size);
	t1 = MPI_Wtime();
	PingPong(x, ProcRank, size);
	t2 = MPI_Wtime();
	dt = t2 - t1;
	if (ProcRank==0)
		printf("Ping Pong executed for %f\n", dt);
    return 0;
}

