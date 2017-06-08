#include <string.h>
#include <stdio.h>
#include <mpi.h>
int main(int argc, char **argv)
{
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;
	int size, rank;
	char str[100];

	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	if(0 == rank){	
		strcpy(str, "hello world");
		printf("Process 0 send %s to Process 1\n", str);
		MPI_Send(str, strlen(str)+1, MPI_CHAR, 1, 99, comm);
	}
	else if(1 == rank) {
		MPI_Recv(str, 100, MPI_CHAR, 0, 99, comm, &status);
		printf("Process %d receive message %s from %d.\n", 1, str, 0);	
	}
	
	MPI_Finalize();
	return 0;
}
