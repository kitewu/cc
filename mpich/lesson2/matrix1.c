#include<stdlib.h>
#include<stdio.h>
#include<time.h>
#include<mpi.h>
#define R1 1000
#define C1 1000
#define R2 1000
#define C2 1000
#define ROOT 0

void print_(int*);				///输出结果
void format(int*, int*, int);	///格式化结果
void save_to_file(int*);		///保存到文件

int main(int argc, char** argv) {
	
	time_t s=0, e=0;
	
    MPI_Comm comm = MPI_COMM_WORLD;
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int i, j, k, count;
    int matrix1[R1][C1];
    int each_col = C2 / size;   ///每个进程有 C2/size列
    int column[each_col][R2];   ///分配给每一个进程 有each_col行，R2列，转置以后的
    int *matrix2=NULL;          ///矩阵2
    int *rbuf=NULL;          	///存储计算结果
	int *rbuf1=NULL;          	///存储最终结果

    ///给第一个矩阵设置值
    count = 1;
    for(i=0; i<R1; i++) {
        for(j=0; j<C1; j++) {
            matrix1[i][j] = count;
        }
    }

    if(rank == ROOT) {
		s = clock();
        ///第二个矩阵申请内存
        matrix2 = (int*)malloc(sizeof(int)*R2*C2);
        ///第二个设置值
        count=1;
        for(i=0; i<R2*C2; i++) {
            matrix2[i]=count;
        }

        ///申请内存
        rbuf = (int*) malloc(sizeof(int)*R1*C2);
		rbuf1 = (int*) malloc(sizeof(int)*R1*C2);
    }

    MPI_Scatter(matrix2, R2*each_col , MPI_INT, column, R2*each_col, MPI_INT, ROOT, comm);
	
    //执行矩阵乘法
    int result[R1][each_col];
    for(i=0; i<R1; i++) {
        for(j=0; j<each_col; j++) {
            result[i][j]=0;
            for(k=0; k<C1; k++) {
                result[i][j] += matrix1[i][k]*column[j][k];
            }
        }
    }

    MPI_Gather(result, R1*each_col, MPI_INT, rbuf, R1*each_col, MPI_INT, ROOT, comm);

    if(ROOT == rank) {	
		///转置，使结果格式化
		format(rbuf, rbuf1, each_col);
		///打印结果到屏幕
		///print_(rbuf1);
		///输出结果到文件
		save_to_file(rbuf1);
	
		free(matrix2);
		free(rbuf);
		free(rbuf1);
		
		e = clock();
		printf("time: %ld ms\n", (e-s)/1000);
    }

    MPI_Finalize();
    return 0;
}

void save_to_file(int* rbuf1){
	int i, j;
	FILE *fpt;
	fpt = fopen("out1", "w");
	for(i=0; i<R1; i++){
		fprintf(fpt, "%d", rbuf1[i*C2]); 
		for(j=1; j<C2; j++)
			fprintf(fpt, " %d", rbuf1[i*C2+j]); 
		fprintf(fpt,"\n"); 
	}
	fclose(fpt);
}

void format(int* rbuf, int* rbuf1, int each_col){
	int count = 0, i, j, k;
	for(i=0; i<R1; i+=each_col){
		for(k=0; k<R1; k++){
			for(j=i; j<i+each_col; j++){
				rbuf1[k*R1+j] = rbuf[count++];
			}
		}
	}
}

void print_(int* rbuf1){
	int i, j;
	for(i=0; i<R1; i++){
		for(j=0; j<C2; j++){
			printf("%d ", rbuf1[i*C2+j]);
		}
		printf("\n");
	}
}
