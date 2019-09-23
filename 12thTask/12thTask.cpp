// 12thTask.cpp: определяет точку входа для консольного приложения.
//

#include "stdafx.h"
#include "mpi.h"
#include <iostream>
#include <math.h>
#include <algorithm>
#include <random>
#include <time.h> 
using namespace std;

struct Vector
{
	//double Values1[8];
	//double Values2[8];
	double dd[16];
};
// Function for random definition of matrix and vector elements
void RandomDataInitialization(double* Matrix, int size)
{
	srand(unsigned(clock()));
	printf("\nA Matrix a on Rank 0: \n");
	for (int i = 0; i<size; i++)
	{
		for (int j = 0; j<size; j++)
		{
			if (j >= i)
				Matrix[i*size + j] = rand() / double(1000);
			else
				Matrix[i*size + j] = 0;
			printf("%0.f  ", Matrix[i*size + j]);
		}
		printf("\n");
	}
}

// Function for memory allocation and data initialization
void ProcessInitialization(double* &a, double* &d,
	int &size, int &newSize, int &ProcNum, int &ProcRank)
{
	MPI_Bcast(&size, 1, MPI_INT, 0, MPI_COMM_WORLD);
	MPI_Bcast(&newSize, 1, MPI_INT, 0, MPI_COMM_WORLD);
	a = new double[size*size];
	d = new double[newSize*size];
	//printf("\nA Matrix: \n");
	for (int i = 0; i<newSize; i++)
	{
		for (int j = 0; j<size; j++)
		{
			d[i*newSize + j] = 0;
			//printf("%0.f  ", d[i*size + j]);
		}
		//printf("\n");
	}
	if (ProcRank == 0)
	{
		RandomDataInitialization(a, size);
	}
}
int main()
{
	int ProcRank, ProcNum;
	double* a;
	double* d;
	MPI_Init(NULL, NULL);
	MPI_Comm_size(MPI_COMM_WORLD, &ProcNum);
	MPI_Comm_rank(MPI_COMM_WORLD, &ProcRank);
	int size = 8; // Sizes of initial matrix 8x8
	int newSize = 2;
	struct Vector vector;
	ProcessInitialization(a, d, size, newSize, ProcNum, ProcRank);
	MPI_Datatype newType, oldTypes[1] = { MPI_DOUBLE };
	MPI_Aint blockLengths[1];
	blockLengths[0] = 16;
	MPI_Aint offsets[1];
	offsets[0] = offsetof(struct Vector, dd);

	MPI_Type_create_struct(1, blockLengths, offsets, oldTypes, &newType);
	MPI_Type_commit(&newType);
	if (ProcRank == 0)
	{
		//fill Vector
		int n = 0;
		for (int i = 1; i<ProcNum; i++)
		{
			for (int j = i; j<size; j += 4)
			{
				for (int k = 0; k<size; k++)
				{
					vector.dd[n*size + k] = a[j*size + k];
				}
				n++;
			}
			n = 0;
			/*printf("\nd filled for proc: \n");
			for (int i = 0; i<newSize; i++)
			{
				for (int j = 0; j<size; j++)
				{
					printf("%0.f  ", vector.dd[i*size + j]);
				}
				printf("\n");
			}*/
			MPI_Send(&vector, 1, newType, i, 0, MPI_COMM_WORLD);
		}

		for (int j = 0; j<size; j += 4)
		{
			for (int k = 0; k<size; k++)
			{
				vector.dd[n*size + k] = a[j*size + k];
			}
			n++;
		}
		printf("\nd Matrix on process %d: \n", ProcRank);
		for (int i = 0; i<newSize; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", vector.dd[i*size + j]);
			}
			printf("\n");
		}
	}
	else
	{
		MPI_Recv(&vector, 1, newType, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		printf("\nd Matrix on process %d: \n", ProcRank);
		for (int i = 0; i<newSize; i++)
		{
			for (int j = 0; j<size; j++)
			{
				printf("%0.f  ", vector.dd[i*size + j]);
			}
			printf("\n");
		}
	}
	/*MPI_Datatype newType, oldTypes[2] = { MPI_DOUBLE , MPI_DOUBLE};
	MPI_Aint blockLengths[2];
	blockLengths[0] = blockLengths[1] = 8;*/
	/*MPI_Aint offsets[2];
	offsets[0] = offsetof(struct Vector, Values1);
	offsets[1] = offsetof(struct Vector, Values2);*/
	//if (ProcRank == 0)
	//{
	//	//fill Vector
	//	for (int i = 1; i<ProcNum; i++)
	//	{
	//		for (int j = i - 1; j<size; j += 4)
	//		{
	//			for (int k = 0; k<size; k++)
	//			{
	//				if (j == i - 1)
	//					vector.Values1[k] = a[j*size + k];
	//				else
	//					vector.Values2[k] = a[j*size + k];
	//			}
	//		}
	//		printf("\nd filled for proc: \n");
	//		for (int i = 0; i<newSize; i++)
	//		{
	//			for (int j = 0; j<size; j++)
	//			{
	//				if (i == 0)
	//					printf("%0.f  ", vector.Values1[j]);
	//				else
	//					printf("%0.f  ", vector.Values2[j]);
	//			}
	//			printf("\n");
	//		}
	//		//ProcNum++;
	//		MPI_Send(&vector, 1, newType, i, 0, MPI_COMM_WORLD);
	//	}
	//}
	//else
	//{
	//	MPI_Recv(d, 1, newType, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	printf("\nd Matrix: \n");
	//	for (int i = 0; i<newSize; i++)
	//	{
	//		for (int j = 0; j<size; j++)
	//		{
	//			if (i == 0)
	//				printf("%0.f  ", vector.Values1[j]);
	//			else
	//				printf("%0.f  ", vector.Values2[j]);
	//		}
	//		printf("\n");
	//	}
	//}
	
	MPI_Finalize();
	return 0;
}

