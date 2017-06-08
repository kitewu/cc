#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
int main(int argc, char** argv) {
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	
	int sbuf[3], rbuf[3], i;
	
	for (i = 0; i < 3; i++) sbuf[i] = rank * 10 + i;
		printf("Process %d has: ", rank);
	for (i = 0; i < 3; i++) 
		printf("%d ", sbuf[i]);
	putchar('\n');
	
	MPI_Allreduce(sbuf, rbuf, 3, MPI_INT, MPI_SUM, MPI_COMM_WORLD);
	
	printf("Total sum of process %d = ", rank);
	for (i = 0; i < 3; i++) 
		printf("%d ",rbuf[i]);
	putchar('\n');
	
	MPI_Finalize();
	return 0;
}
