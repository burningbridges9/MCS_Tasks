//// 9thTask.cpp: определяет точку входа для консольного приложения.


#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;

struct Pair
{
	double value;
	int index;
};

void DataInit(double* &x, double* &xPart, int* &indexes, int* &indexesPart,
	int ProcNum, int ProcRank, int &size, int &chunk)
{
	chunk = size / ProcNum;
	//send this to define size of local array
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
	x = new double[size];
	xPart = new double[chunk];
	indexes = new int[size];
	indexesPart = new int[chunk];
	// подготовка данных
	if (ProcRank == 0)
	{
		//srand(unsigned(clock()));
		printf("\nValue - Index: \n");
		for (int i = 0; i<size; i++)
		{
			x[i] =100 - rand() / double(1000);
			indexes[i] = i;			
			printf("%f - %d||  ", x[i], indexes[i]);
		}
		printf("\n");
	}
}

void DataDistribution(double* &x, double* &xPart, int* &indexes, int* &indexesPart,
	int chunk, int ProcRank)
{
	MPI_Scatter(x + chunk*ProcRank, chunk, MPI_DOUBLE,
		xPart, chunk, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
	MPI_Scatter(indexes + chunk*ProcRank, chunk, MPI_INT,
		indexesPart, chunk, MPI_DOUBLE,
		0, MPI_COMM_WORLD);
	printf("\nChunked value - index vector on process %d: \n", ProcRank);
	for (int i = 0; i<chunk; i++)
	{
		printf("%f - %d; ", xPart[i], indexesPart[i]);
	}
	printf("\n");
}


void Calculs(double* &xPart, int* &indexesPart,
	int chunk, int ProcRank, Pair & localMin, Pair &globalMin)
{
	localMin.value = xPart[0];
	localMin.index = indexesPart[0];
	for (int i = 1; i < chunk; i++)
		if (localMin.value > xPart[i])
		{
			localMin.value = xPart[i];
			localMin.index = indexesPart[i];
		}
	printf("\nFound localMin.Val = %f , localMin.index = %d; on process %d\n", 
		localMin.value, localMin.index, ProcRank);
	MPI_Reduce(&localMin, &globalMin, 1,
		MPI_DOUBLE_INT, MPI_MINLOC, 
		0, MPI_COMM_WORLD);
}




int main()
{
	int ProcRank, ProcNum, chunk;

	Pair globalMin;
	Pair localMin;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	double * x;
	double * xPart;
	int * indexes;
	int * indexesPart;
	MPI_Status st;
	int size = 2 * ProcNum;
	//распределение данных по процессам
	DataInit(x, xPart, indexes, indexesPart, ProcNum, ProcRank, size, chunk);
	DataDistribution(x, xPart, indexes, indexesPart, chunk, ProcRank);
	Calculs(xPart, indexesPart, chunk, ProcRank, localMin, globalMin);

	//check
	if (ProcRank == 0)
	{
		printf("\nParallel min index result: %d\n", globalMin.index);
		Pair nonParallelMin;
		nonParallelMin.value = x[0];
		nonParallelMin.index = indexes[0];
		for (int i = 1; i < size; i++)
			if (nonParallelMin.value > x[i])
			{
				nonParallelMin.value = x[i];
				nonParallelMin.index = indexes[i];
			}
		printf("\nNon-parallel localMin.Val = %f , localMin.index = %d\n",
			nonParallelMin.value, nonParallelMin.index );
	}
	MPI_Finalize();
	return 0;
}
//
// 9thTask.cpp: определяет точку входа для консольного приложения.
//

//#include "stdafx.h"
//#include "mpi.h"
//#include <iostream>
//#include <math.h>
//#include <algorithm>
//#include <random>
//#include <time.h> 
//using namespace std;

////struct Pair
////{
////	double value;
////	int index;
////};
//
//void DataInit(struct{double value; double index;}* &x, struct{double value; double index;}* &xPart, int ProcNum, int ProcRank, int &size, int &chunk)
//{
//	chunk = size / ProcNum;
//	//send this to define size of local array
//	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
//	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
//	x = new struct{double value; double index;}[size];
//	xPart = new struct{double value; double index;}[chunk];
//	// подготовка данных
//	if (ProcRank == 0)
//	{
//		srand(unsigned(clock()));
//		printf("\nstruct{double value; double index;} Vector: \n");
//		for (int i = 0; i<size; i++)
//		{
//			x[i].value = rand() / double(1000);srand(unsigned(clock()));
//			x[i].index = i;
//
//			printf("%f - %d; ", x[i].value, x[i].index);
//		}
//		printf("\n");
//	}
//}
//
//void DataDistribution(struct{double value; double index;}* &x, struct{double value; double index;}* &xPart, int chunk, int ProcRank)
//{
//	MPI_Scatter(x + chunk*ProcRank, chunk * sizeof(struct{double value; double index;}), MPI_DOUBLE,
//		xPart, chunk, MPI_DOUBLE,
//		0, MPI_COMM_WORLD);
//
//	printf("\nChunked vector on process %d: \n", ProcRank);
//	for (int i = 0; i<chunk; i++)
//	{
//		printf("%f - %d; ", xPart[i].value, xPart[i].index);
//	}
//	printf("\n");
//}
//
//
//void Calculs(struct{double value; double index;}* &xPart, int chunk, int ProcRank, struct{double value; double index;} & localMin, struct{double value; double index;} &globalMin)
//{
//	localMin = xPart[0];
//	for (int i = 1; i < chunk; i++)
//		if (localMin.value < xPart[i].value)
//		{
//			localMin.value = xPart[i].value;
//			localMin.index = xPart[i].index;
//		}
//	printf("\nlocalMin.Val = %f , localMin.index = %d; on process %d\n",
//		localMin.value, localMin.index, ProcRank);
//	MPI_Reduce(&localMin, &globalMin, 1,
//		MPI_DOUBLE_INT, MPI_MINLOC,
//		0, MPI_COMM_WORLD);
//}




//int main()
//{
//	int ProcRank, ProcNum, chunk;
//	
//	MPI_Status st;
//	srand(unsigned(clock()));
//	MPI_Init(NULL, NULL);
//	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
//	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
//	int size = 10;
//	chunk = size / ProcNum;
//	//send this to define size of local array
//	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
//	MPI_Bcast(&chunk, 1, MPI_INT, 0, MPI_COMM_WORLD);
//	struct { double value; int index; }
//	sendbuf[100], recvbuf[100];
//	int root = 3, myrank, error, i;
//	for (i = 0; i < 100; ++i) {
//		sendbuf[i].value = rand() / double(1000);
//		sendbuf[i].index = i;
//	}
//	for (int i = 0; i < 100; i++)
//	{
//		printf("%f - %d; ", sendbuf[i].value, sendbuf[i].index);
//	}
//	MPI_Reduce(sendbuf, recvbuf,
//		100, MPI_DOUBLE_INT, MPI_MINLOC, 0, MPI_COMM_WORLD);
//	if (ProcRank == 0)
//	{
//		for (int i = 0; i < 100; i++)
//		{ 
//			printf("%f - %d; ", recvbuf[i].value, recvbuf[i].index);
//		}
//	}
//	MPI_Finalize();
//	return 0;
//}

