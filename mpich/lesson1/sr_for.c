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

	for(int i=0; i<size; i++){
		if(0 == i && 0 == rank){
			strcpy(str, "hello world");
			printf("Process 0 send %s to Process 1\n", str);
			MPI_Send(str, strlen(str)+1, MPI_CHAR, 1, 99, comm);
		}else if(i>0 && i<size && i == rank){
			MPI_Recv(str, 100, MPI_CHAR, i-1, 99, comm, &status);		
			printf("Process %d receive message %s from %d.\n", i, str, i-1);
			if(i < size-1){	
				MPI_Send(str, strlen(str)+1, MPI_CHAR, i+1, 99, comm);	
				printf("Process %d send %s to Process %d\n", i, str, i+1);
			}
		}
	}	
	MPI_Finalize();
	return 0;
}
