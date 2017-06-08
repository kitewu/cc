#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<math.h>
#include<mpi.h>
#define PI 3.14159265358979
#define ROOT 0
int main(int argc, char** argv){
	time_t s=0, e=0;///时间统计
	///MPI初始化
	MPI_Comm comm = MPI_COMM_WORLD;
	int size, rank;
	MPI_Init(&argc, &argv);
	MPI_Comm_size(comm, &size);
	MPI_Comm_rank(comm, &rank);

	int j;
	double a=0, b=100*PI;		///区间，单位为弧度
	double *arr_res=NULL;		///保存计算结果数组
	double part_len=(b-a)/size;	///每一小段的长度
	double sum_local=0, sum=0;	///每一进程的计算结果和最终计算结果
	double width=0.000001;		///每一小段的宽度，即计算精度
	///在根进程中申请结果数组
	if(ROOT == rank){
		s = clock();
		arr_res = (double*) malloc(sizeof(double)*size);
	}
	///每一段求积分
	double begin=rank*part_len, end=(rank+1)*part_len;
	for(; begin<end; begin+=width){
		sum_local += fabs(cos(begin)*width);
	//	sum_local += cos(begin)*width;
	}
	///打印每一段的积分结果
	printf("Process %d integrate is : %lf\n", rank, sum_local);
	///每一段将积分结果发到根进程
	MPI_Gather(&sum_local, 1, MPI_DOUBLE, arr_res, 1, MPI_DOUBLE, ROOT, comm);
	///根进程计算最终结果
	if(ROOT == rank){
		for(j=0; j<size; j++)
			sum += arr_res[j];
		printf("cos(x) integarte is %lf\n", sum);
		free(arr_res);
	
		e = clock();
		printf("time: %ldms\n", (e-s)/1000);
	}
	///MPI结束
	MPI_Finalize();
	return 0;
}

	
