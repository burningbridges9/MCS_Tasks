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

void PingPong(double* &x, double *&y, int ProcRank, int &size, int count)
{
	
	int limit = 3;
	int partner = (ProcRank + 1) % 2;
	while (count < limit) {
		if (ProcRank == count % 2) 
		{
			count++;
			if (count == 1)
				MPI_Send(x, size, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD);
			if (count == 3)
				MPI_Recv(y, size, MPI_DOUBLE, partner, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//printf("%d sent and incremented ping_pong_count "
			//	"%d to %d\n", ProcRank, count,
			//	partner);
		}
		else 
		{
			count++;
			MPI_Recv(y, size, MPI_DOUBLE, partner, 0,
				MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Send(x, size, MPI_DOUBLE, partner, 0,
				MPI_COMM_WORLD);
			//printf("%d sent and incremented ping_pong_count "
			//	"%d to %d\n", ProcRank, count,
			//	partner);
		}
	}
}

void PingPing(double* &x, double *&y, int ProcRank, int &size)
{
	int partner = (ProcRank + 1) % 2;
	MPI_Request request1, request2;
	MPI_Status status;
	MPI_Irecv(y, 10, MPI_DOUBLE, partner, 123, MPI_COMM_WORLD, &request1);
	MPI_Isend(x, 10, MPI_DOUBLE, partner, 123, MPI_COMM_WORLD, &request2);
	MPI_Wait(&request1, &status);
	MPI_Wait(&request2, &status);
}


int main()
{
	int count = 0;
	int ProcRank, ProcNum;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	double * x;
	double * y; 
	double t1, t2, dt;
	//PingPong
	for (int size = 100; size <= 10000; size += 100)
	{
		count = 0;
		DataInit(x, ProcNum, ProcRank, size);
		DataInit(y, ProcNum, ProcRank, size);
		t1 = MPI_Wtime();
		PingPong(x,y, ProcRank, size, count);
		t2 = MPI_Wtime();
		dt = t2 - t1;
		if (ProcRank == 0)
			printf("Ping Pong executed for time %f s and size = %i \n", dt, size);

		delete x;
		delete y;
	}
	//PingPing
	/*for (int size = 100; size <= 10000; size += 100)
	{
		DataInit(x, ProcNum, ProcRank, size);
		DataInit(y, ProcNum, ProcRank, size);
		t1 = MPI_Wtime();
		PingPing(x,y, ProcRank, size);
		t2 = MPI_Wtime();
		dt = t2 - t1;
		if (ProcRank == 0)
			printf("Ping Ping executed for time %f s and size = %i \n", dt, size);

		delete x;
		delete y;
	}*/
	MPI_Finalize();
    return 0;
}

