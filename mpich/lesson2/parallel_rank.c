#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>
#include<mpi.h>
#define RANDOM_MAX 20

typedef struct Node{
	int owner;
	int number;
}Node;

void sort_node(Node *arr, int size){
	int i, j;
	for(i=0; i<size-1; i++){
		for(j=0; j<size-1-i; j++){
			if(arr[j].number > arr[j+1].number){
				Node t = arr[j];
				arr[j] = arr[j+1];
				arr[j+1] = t;
			}
		}
	}
}

int main(int argc, char** argv){
	MPI_Comm comm = MPI_COMM_WORLD;	
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	int i, number, *arr_number=NULL, *sbuf=NULL, range;
	Node *arr_node=NULL;	
//	srand((unsigned)time(NULL));
//	number = rand() % RANDOM_MAX;
	number = size-rank-1;
	if(0 == rank){
		arr_number = (int*) malloc(sizeof(int) * size);
	}
	
	MPI_Gather(&number, 1, MPI_INT, arr_number, 1, MPI_INT, 0, comm);
	
	if(0 == rank){
		arr_node = (Node*) malloc(sizeof(Node) * size);
		memset(arr_node, 0, sizeof(arr_node));
		for(i=0; i<size; i++){
			Node node = {i, arr_number[i]};
			arr_node[i] = node;
		}
		
		sort_node(arr_node, size);
		
		sbuf = (int*) malloc(sizeof(int) * size);
		memset(sbuf, 0, sizeof(int));
		
		for(i=0; i<size; i++){
			sbuf[arr_node[i].owner] = i;
		}
	}
	MPI_Scatter(sbuf, 1, MPI_INT, &range, 1, MPI_INT, 0, MPI_COMM_WORLD);
	printf("(process, range, number) : (%d, %d, %d)\n", rank, range, number);
	
	if(0 == rank){
		free(arr_node);
		free(arr_number);
		free(sbuf);
	}
	
	MPI_Finalize();
	return 0;
}