// ThirdTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
using namespace std;


int main()
{
	int size, rank;
	MPI_Status st;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	int  N = 10;
	double * x = new double[N];	
	
	if (rank == 1)
	{
		// prepare
		printf("\nArray values on 1st process:\n");
		for (int i = 0; i < N; i++)
		{
			x[i] = rand() % 10;
			printf("x[%d] = %f ", i, x[i]);
		}
		// send to others
		for (int i = 0; i < size; i++)
		{
			if (i == 1)
				continue;
			MPI_Send(x, N, MPI_DOUBLE,
				i, 1, MPI_COMM_WORLD);
		}
	}
	else
	{
		//get from first proc
		//check for buffer
		int buf = 0;
		MPI_Status status;
		MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &st);
		int GetN = 0;
		MPI_Get_count(&st, MPI_DOUBLE, &GetN);
		printf("GetN = %d\n", GetN);
		printf("\nStatus messages:\nMPI_SOURCE: %d\nMPI_TAG: %d\nMPI_ERROR: %d\n", st.MPI_SOURCE, st.MPI_TAG, st.MPI_ERROR);
		MPI_Recv(x, GetN, MPI_DOUBLE,
			1, 1, MPI_COMM_WORLD, &st);
		printf("\nProcess %d:\n", rank);
		for (int i = 0; i < N; i++)
		{			
			printf("x[%d] = %f ", i, x[i]);
		}
	}

	delete x;
	MPI_Finalize();
	return 0;
}

