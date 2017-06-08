#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<mpi.h>
#define LENGTH 1000
#define PROS 2

int part_size;
int* initMatrix(int *M, int size);
void MatrixMulti(int arr_part_a[part_size][part_size], 
				 int arr_part_b[part_size][part_size],
				 int part_result[part_size][part_size]);
void sendDataToEachProc(int* M, int* order, 
						int arr_part[part_size][part_size], 
						MPI_Comm comm);
void showMatrix(int*, int, int);
int* formatResult(int *Ma, int *Mb);
void saveToFile(int* M);
			   
int main(int argc, char*argv[]){
	time_t s=0, e=0;
	
    int size, rank;
	MPI_Comm comm = MPI_COMM_WORLD;
	MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
	int *Ma=NULL;	///����A
	int *Mb=NULL;	///����B
	part_size = LENGTH / PROS;	///ÿ������ֵõĿ�ߵĴ�С
	int arr_part_a[part_size][part_size];///�洢����A�ֵĿ�
	int arr_part_b[part_size][part_size];///�洢����B�ֵĿ�
	
	int part_result[part_size][part_size];///�洢�������,��һ��
	int part_result_2[part_size][part_size];///�洢�������,�ڶ���
	
	///��ʼ�����ַ�����
	if(0 == rank){
		s = clock();
		
		Ma = initMatrix(Ma, LENGTH*LENGTH);
		Mb = initMatrix(Mb, LENGTH*LENGTH);
		
		///��һ�η�������
		///�������A���ݷַ������̵�˳��
		int send_order_a[4] = {0, 1, 3, 2};
		///��ÿһ�����̷���A��������
		sendDataToEachProc(Ma, send_order_a, arr_part_a, comm);
		///�������B���ݷַ������̵�˳��
		int send_order_b[4] = {0, 3, 2, 1};
		///��ÿһ�����̷���B��������
		sendDataToEachProc(Mb, send_order_b, arr_part_b, comm);
	}
	
	///�����������������,��һ��
	if(0 != rank){
		MPI_Recv(arr_part_a, part_size*part_size, MPI_INT, 0, 99, comm, &status);
		MPI_Recv(arr_part_b, part_size*part_size, MPI_INT, 0, 99, comm, &status);
	}
	
	///����˷���һ��
	MatrixMulti(arr_part_a, arr_part_b, part_result);
	
	///�ڶ��η�������
	if(0 == rank){
		///�������A���ݷַ������̵�˳��
		int send_order_a[4] = {1, 0, 2, 3};
		///��ÿһ�����̷���A��������
		sendDataToEachProc(Ma, send_order_a, arr_part_a, comm);
		///�������B���ݷַ������̵�˳��
		int send_order_b[4] = {2, 1, 0, 3};
		///��ÿһ�����̷���B��������
		sendDataToEachProc(Mb, send_order_b, arr_part_b, comm);
	}
	
	///�����������������,�ڶ���
	if(0 != rank){
		MPI_Recv(arr_part_a, part_size*part_size, MPI_INT, 0, 99, comm, &status);
		MPI_Recv(arr_part_b, part_size*part_size, MPI_INT, 0, 99, comm, &status);
	}
	
	///����˷��ڶ���
	MatrixMulti(arr_part_a, arr_part_b, part_result_2);
	
	///����ӷ�
	int i, j;
	for(i=0; i<part_size; i++){
		for(j=0; j<part_size; j++){
			part_result[i][j] += part_result_2[i][j];
		}
	}
	
	///���������͵�0����
	MPI_Gather(part_result, part_size*part_size, MPI_INT, Ma, part_size*part_size, MPI_INT, 0, comm);
	
	///�ڸ����̸�ʽ��������
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

///���������ļ�
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

///��ʽ�����
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

///��ӡ����
void showMatrix(int* M, int row, int col){
	int i, j;
	for(i=0; i<row; i++){
		for(j=0; j<col; j++){
			printf("%d ", M[i*col+j]);
		}
		printf("\n");
	}
}

///����˷�
void MatrixMulti(int arr_part_a[part_size][part_size], 
				 int arr_part_b[part_size][part_size],
				 int part_result[part_size][part_size])
{
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

///��ʼ������
int* initMatrix(int *M, int size){
	int i;
	M = (int*)malloc(sizeof(int)*size);
	for(i=0; i<size; i++){
		M[i] = 1;
	}
	return M;
}

///�����ݷַ���ÿһ������
void sendDataToEachProc(int* M, int* order, 
						int arr_part[part_size][part_size], 
						MPI_Comm comm)
{
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
			if(0 == order[i*PROS+j])///�����̲����ڴ渴��
				memcpy(arr_part, arr_temp, sizeof(int)*part_size*part_size);
			else///�������̲��÷��͵ķ�ʽ
				MPI_Send(arr_temp, part_size*part_size, MPI_INT, order[i*PROS+j], 99, comm);
			base_c += part_size;
		}
		base_r += part_size;
	}
}