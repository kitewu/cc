#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<mpi.h>
int main(int argc, char** argv){
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;	
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);
	
	int arr[1000], i, max, local_max, *begin=NULL, *maxinums=NULL, each=1000/size;
	if(rank == 0){
		for(i=1; i<=1000; i++)
			arr[i-1] = i;
		for(i=1; i<size; i++){
			begin = arr + i * each;
			MPI_Send(begin, each, MPI_INT, i, 99, comm);
		}
		maxinums = (int *)malloc(sizeof(int) * size);
	}else{
		MPI_Recv(arr, each, MPI_INT, 0, 99, comm, &status);		
	}

	//find max value in each process	
	local_max = arr[0];
	for(i = 0; i<each; i++){
		if(local_max < arr[i])
			local_max = arr[i];
	}
	printf("Process %d max value is %d.\n", rank, local_max);
	if(rank != 0){
		MPI_Send(&local_max, 1, MPI_INT, 0, 100, comm);
	}else{
		maxinums[0] = local_max;
		for(i=1; i<size; i++){
			MPI_Recv(&maxinums[i], 1, MPI_INT, i, 100, comm, &status);
		}
		max = maxinums[0];
		for(i=0 ;i<size; i++){
			if(max < maxinums[i])
				max = maxinums[i];
		}
		printf("max num is %d\n", max);
		free(maxinums);
	}

	MPI_Finalize();
	return 0;
}

	
