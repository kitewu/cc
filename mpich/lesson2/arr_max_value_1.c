#include<stdio.h>
#include<stdlib.h>
#include<mpi.h>
int main(int argc, char** argv){
	MPI_Comm comm = MPI_COMM_WORLD;
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);
	
	
	int i, max, local_max, each=1000/size;
	int rbuf[each];		//每个进程接收数据
	int *sbuf=NULL;		//0进程存储所有的数据
	int *resbuf=NULL;	//用于存储所有进程返回的结果的数组
	
	
	//在根进程里设置数据
	if(rank == 0){
		sbuf = (int*) malloc(sizeof(int) * 1000);
		resbuf = (int*) malloc(sizeof(int) * size);
		for(i=1; i<=1000; i++)
			sbuf[i-1] = i;
	}
	
	//根进程分发到每一个进程
	MPI_Scatter(sbuf, each, MPI_INT, rbuf, each, MPI_INT, 0, comm);

	//每一个进程中求最大值
	local_max = rbuf[0];
	for(i=1; i<each; i++){
		if(local_max < rbuf[i])
			local_max = rbuf[i];
	}
	
	printf("Process %d max value is %d\n", rank, local_max);
	
	//收集每一进程求的结果
	MPI_Gather(&local_max, 1, MPI_INT, resbuf, 1, MPI_INT, 0, comm);
	
	//根进程求最终结果
	if(rank == 0){
		max = resbuf[0];
		for(i = 1; i<size; i++){
			if(max < resbuf[i])
				max = resbuf[i];
		}
		printf("The max value is : %d\n", max);
		free(resbuf);
		free(sbuf);
	}
	
	
	MPI_Finalize();
	return 0;
}

	
