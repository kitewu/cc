#include <stdio.h>
#include <mpi.h>
int main(int argc, char **argv)
{
	int size, rank, var;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	var = rank * 20;
	printf("size = %d, rank = %d\n", size, rank);
	printf("var = %d\n", var);
	
	MPI_Finalize();
	return 0;
}
