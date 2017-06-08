#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<mpi.h>
#define LENGTH 1000
#define PROS 2

int part_size;
int* initMatrix(int*, int);
void MatrixMulti(int**, int**, int**);
void sendDataToEachProc(int* , int*, int**, MPI_Comm);
void showMatrix(int*, int, int);
int* formatResult(int*, int*);
void saveToFile(int*);
			   
int main(int argc, char*argv[]){
	time_t s=0, e=0;
	
    int size, rank;
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int *Ma=NULL;	///矩阵A
	int *Mb=NULL;	///矩阵B
	part_size = LENGTH / PROS;	///每个矩阵分得的块边的大小
	int arr_part_a[part_size][part_size];///存储矩阵A分的块
	int arr_part_b[part_size][part_size];///存储矩阵B分的块
	
	int part_result[part_size][part_size];///存储块计算结果,第一次
	int part_result_2[part_size][part_size];///存储块计算结果,第二次
	
	///初始化及分发数据
	if(0 == rank){
		s = clock();
		
		Ma = initMatrix(Ma, LENGTH*LENGTH);
		Mb = initMatrix(Mb, LENGTH*LENGTH);
		
		///第一次分配数据
		///定义矩阵A数据分发到进程的顺序
		int send_order_a[4] = {0, 1, 3, 2};
		///给每一个进程分配A矩阵数据
		sendDataToEachProc(Ma, send_order_a, (int**)arr_part_a, comm);
		///定义矩阵B数据分发到进程的顺序
		int send_order_b[4] = {0, 3, 2, 1};
		///给每一个进程分配B矩阵数据
		sendDataToEachProc(Mb, send_order_b, (int**)arr_part_b, comm);
	}
	
	///除根进程外接收数据,第一次
	if(0 != rank){
		MPI_Recv(arr_part_a, part_size*part_size, MPI_INT, 0, 99, comm, &status);
		MPI_Recv(arr_part_b, part_size*part_size, MPI_INT, 0, 99, comm, &status);
	}
	
	///矩阵乘法第一次
	MatrixMulti((int**)arr_part_a, (int**)arr_part_b, (int**)part_result);
	
	///第二次分配数据
	if(0 == rank){
		///定义矩阵A数据分发到进程的顺序
		int send_order_a[4] = {1, 0, 2, 3};
		///给每一个进程分配A矩阵数据
		sendDataToEachProc(Ma, send_order_a, (int**)arr_part_a, comm);
		///定义矩阵B数据分发到进程的顺序
		int send_order_b[4] = {2, 1, 0, 3};
		///给每一个进程分配B矩阵数据
		sendDataToEachProc(Mb, send_order_b, (int**)arr_part_b, comm);
	}
	
	///除根进程外接收数据,第二次
	if(0 != rank){
		MPI_Recv(arr_part_a, part_size*part_size, MPI_INT, 0, 99, comm, &status);
		MPI_Recv(arr_part_b, part_size*part_size, MPI_INT, 0, 99, comm, &status);
	}
	
	///矩阵乘法第二次
	MatrixMulti((int**)arr_part_a, (int**)arr_part_b, (int**)part_result_2);
	
	///矩阵加法
	int i, j;
	for(i=0; i<part_size; i++){
		for(j=0; j<part_size; j++){
			part_result[i][j] += part_result_2[i][j];
		}
	}
	
	///计算结果发送到0进程
	MPI_Gather(part_result, part_size*part_size, MPI_INT, Ma, part_size*part_size, MPI_INT, 0, comm);
	
	///在根进程格式化计算结果
	if(0 == rank){
		Mb = formatResult(Ma, Mb);
		saveToFile(Mb);
		free(Ma);
		free(Mb);
		e = clock();
		printf("time: %ld ms\n", (e-s)/1000);
	}
	
	MPI_Finalize();
	return 0;
}

///输出结果到文件
void saveToFile(int* M){
	FILE *fpt;
	fpt = fopen("out3", "w");
	int i, j;
	for(i=0; i<LENGTH; i++){
		fprintf(fpt, "%d", M[i*LENGTH]); 
		for(j=0; j<LENGTH; j++){
			fprintf(fpt, " %d", M[i*LENGTH+j]); 
		}
	}
	fclose(fpt);
}

///格式化结果
int* formatResult(int *Ma, int *Mb){
	int i, j, k, m;
	int base_r = 0;
	int base_c = 0;
	int *begin;
	for(i=0; i<PROS; i++){
		base_c = 0;
		for(j=0; j<PROS; j++){
			begin = Ma + (i*PROS+j)*part_size*part_size;
			for(k=0; k<part_size; k++){
				for(m=0; m<part_size; m++){
					Mb[(base_r+k)*LENGTH+base_c+m] = begin[k*part_size+m];
				}
			}
			base_c += part_size;
		}
		base_r += part_size;
	}
	return Mb;
}

///打印矩阵
void showMatrix(int* M, int row, int col){
	int i, j;
	for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			printf("%d ", M[i*col+j]);
		}
		printf("\n");
	}
}

///矩阵乘法
void MatrixMulti(int** arr_part_a, int** arr_part_b, int** part_result){
	int i, j, k;
	for(i=0; i<part_size; i++){
		for(j=0; j<part_size; j++){
			part_result[i][j] = 0;
			for(k=0; k<part_size; k++){
				part_result[i][j] += arr_part_a[i][k] * arr_part_b[k][j];
			}
		}
	}
}

///初始化矩阵
int* initMatrix(int *M, int size){
	int i;
	M = (int*)malloc(sizeof(int)*size);
	for(i=0; i<size; i++){
		M[i] = 1;
	}
	return M;
}

///将数据分发到每一个进程
void sendDataToEachProc(int* M, int* order, int** arr_part, MPI_Comm comm){
	int i, j, m, n;
	int arr_temp[part_size][part_size];
	int base_r = 0;
	int base_c = 0;
	for(i=0; i<PROS; i++){
		base_c = 0;
		for(j=0; j<PROS; j++){
			for(m=0; m<part_size; m++)
				for(n=0; n<part_size; n++)
					arr_temp[m][n] = M[(base_r+m)*LENGTH+base_c+n];
			if(0 == order[i*PROS+j])///根进程采用内存复制
				memcpy(arr_part, arr_temp, sizeof(int)*part_size*part_size);
			else///其它进程采用发送的方式
				MPI_Send(arr_temp, part_size*part_size, MPI_INT, order[i*PROS+j], 99, comm);
			base_c += part_size;
		}
		base_r += part_size;
	}
}