#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
int main(int argc, char** argv){
	MPI_Comm comm = MPI_COMM_WORLD;	
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	int i, sbuf[3], *rbuf=NULL;
	if(0 == rank)
		rbuf = (int *) malloc(sizeof(int)*3*size);
	
	for(i=0; i<3; i++)
		sbuf[i] = rank*10+i;
	
	MPI_Gather(sbuf, 3, MPI_INT, rbuf, 3, MPI_INT, 0, comm);
	
	if(0 == rank){
		printf("Process 0 receive:");
		for(i=0; i<size*3; i++)
			printf("%d ", rbuf[i]);
		printf("\n");
		free(rbuf);
	}

	MPI_Finalize();
	return 0;
}

	
