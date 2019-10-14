// FirstTask.cpp: определяет точку входа для консольного приложения.
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
	int size, rank, recv;
	MPI_Status st;

	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Hello from process %d\nNumber of all processes is %d\n\n", rank, size);
		

	MPI_Finalize();
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	printf("Hello from process %d\nNumber of all processes is %d\n\n", rank, size);
    return 0;
}

