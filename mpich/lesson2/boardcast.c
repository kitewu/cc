#include<stdio.h>
#include<mpi.h>
int main(int argc, char** argv){
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;	
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);


	int i, arr[3];
	if(rank == 0){
		for(i=0; i<3; i++){
			arr[i] = i+1;
		}
	}
	MPI_Bcast(arr, 3, MPI_INT, 0, comm);

	printf("Process %d receives: ", rank);
	for(int i=0; i<3; i++)
		printf("%d ", arr[i]);
	printf("\n");


	MPI_Finalize();
	return 0;
}
