/**
 *在根进程计算每个进程所分的行数和列数
 *将计算结果分配到每一个进程，初始化rows和cols
 *初始化每一个进程中的存储矩阵和结果矩阵
 *将数据分发下去
 *计算矩阵相乘
 *收集结果
 *格式化结果
 */
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<time.h>
#include<mpi.h>
#define R1 7
#define C1 5
#define R2 5
#define C2 7
#define ROOT 0

int main(int argc, char **argv) {
    ///MPI初始化
    MPI_Comm comm = MPI_COMM_WORLD;
    MPI_Status status;
    int size, rank;
    MPI_Init(&argc, &argv);
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);

    int i, j, k, count;
    int *matrix1=NULL;///矩阵1
    int *matrix2=NULL;///矩阵2

    int *arr_dis_rows=NULL; ///用于存储分给每一个进程的行数
    int *arr_dis_cols=NULL; ///用于存储分给每一个进程的列数

    int m1_pros=3;  ///矩阵1分得的进程数
    int m2_pros=2;  ///矩阵2分得的进程数

    int rows=0;     ///每一个进程 矩阵1分得的行数，值由ROOT进程确定,可能出现每一进程分得的行数不相等情况
    int cols=0;     ///每一个进程 矩阵2分得的行数(实际应是列数，这里对矩阵2转置，便于计算，也就是行数)，值由ROOT进程确定,可能出现每一进程分得的行数不相等情况
	
	int *result=NULL;	///存储最终结果
	
    if(rank == ROOT) {
        ///矩阵申请空间并赋值
		count=1;
        matrix1 = (int*) malloc(sizeof(int)*R1*C1);
        for(i=0; i<R1*C1; i++)
            matrix1[i]=count++;
		count=1;
        matrix2 = (int*) malloc(sizeof(int)*C2*R2);
        for(i=0; i<R2*C2; i++)
            matrix2[i]=count++;

        ///求分配给进程的行
        count=0;
        arr_dis_rows = (int*) malloc(sizeof(int)*size);
        int each = R1 / m1_pros;///每一进程的行数
        int left = R1 % m1_pros;///余数
        for(i=0; i<m1_pros; i++) {
            for(j=0; j<m2_pros; j++) {
                arr_dis_rows[count++] = each;
            }
        }
        for(j=0; j<m2_pros; j++) {
            arr_dis_rows[--count] += left;
        }

        ///求分配给进程的列
        count=0;
        arr_dis_cols = (int*) malloc(sizeof(int)*size);
        each = C2 / m2_pros;///每一进程的列数
        left = C2 % m2_pros;///余数
        for(i=0; i<m2_pros; i++) {
            arr_dis_cols[count++] = each;
        }
        arr_dis_cols[--count] += left;
        count=m2_pros;
        for(i=1; i<m1_pros; i++) {
            for(j=0; j<m2_pros; j++) {
                arr_dis_cols[count++]=arr_dis_cols[j];
            }
        }
		///申请结果内存空间
		result=(int*) malloc(sizeof(int)*R1*C2);
   }

    ///将每一进程的分得的行数和列数分配下去
    MPI_Scatter(arr_dis_rows, 1, MPI_INT, &rows, 1, MPI_INT, ROOT, comm);
    MPI_Scatter(arr_dis_cols, 1, MPI_INT, &cols, 1, MPI_INT, ROOT, comm);

    ///每一进程根据分配的行数和列数申请空间
    int arr_rows[rows][C1];	///存储分配的行
    int arr_cols[cols][R2];	///存储分配的列
	int res[rows][cols];	///存储相乘计算结果

    ///分配数据
    if(rank == ROOT) {
        ///给每一进程分配行，行数根据之前求得结果来分
        int *begin = matrix1;
        count=0;
        int pro_id = 0;
        for(i=0; i<size; i+=m2_pros) {
            count = arr_dis_rows[i] * C1;
            for(j=0; j<m2_pros; j++){
				if(0 == pro_id){
					pro_id++;
					continue;
				}
				MPI_Send(begin, count, MPI_INT, pro_id++, 99, comm);
			}
            begin += count;
        }
        ///给每一进程分配列，列数根据之前求得结果来分
        pro_id = 0;
        for(i=0; i<m1_pros; i++) {
            begin = matrix2;
            for(j=0; j<m2_pros; j++) {
                count = arr_dis_cols[j] * R2;
				if(0 == pro_id){
					begin += count;
					pro_id++;
					continue;
				}
                MPI_Send(begin, count, MPI_INT, pro_id++, 100, comm);
                begin += count;
            }
        }
		
		///进程0采用内存拷贝的方式
		memcpy(arr_rows, matrix1, sizeof(int)*rows*C1);
		memcpy(arr_cols, matrix2, sizeof(int)*cols*R2);
    }
	
	///除进程0外接收数据
	if(0 != rank){
		MPI_Recv(arr_rows, rows*C1, MPI_INT, ROOT, 99, comm, &status);
		MPI_Recv(arr_cols, cols*R2, MPI_INT, ROOT, 100, comm, &status);
	}
	
	
	///输出调试信息
	printf("------------------process %d:", rank);
	printf("(%d, %d, %d)\n", rank, rows, cols);
	for(i=0; i<rows; i++){
		for(j=0; j<C1; j++){
			printf("%d ", arr_rows[i][j]);
		}
		printf("\n");
	}
	printf("---------\n");
	for(i=0; i<cols; i++){
		for(j=0; j<R2; j++){
			printf("%d ", arr_cols[i][j]);
		}
		printf("\n");
	}
	
	///计算矩阵相乘
	for(i=0; i< rows; i++){
		for(j=0; j<cols; j++){
			res[i][j]=0;
			for(k=0; k<C1; k++){ 
				res[i][j] += arr_rows[i][k] * arr_cols[j][k];
			}
		}
	}
	
	/*
	printf("------------------process %d:\n", rank);
	printf("(%d, %d, %d)\n", rank, rows, cols);
	for(i=0; i<rows; i++){
		for(j=0; j<cols; j++){
			printf("%d ", res[i][j]);
		}
		printf("\n");
	}
	*/
	
	
	if(rank != 0)
		MPI_Send(res, rows*cols, MPI_INT, ROOT, 101, comm);
	
	///收集结果
	///MPI_Gather(res, rows*cols, MPI_INT, result, rows*cols, MPI_INT, ROOT, comm);
	
	///根进程处理结果
	if(rank == ROOT){
		
		/*
		printf("prcess %d\n", ROOT);
		for(i=0; i<rows; i++){
			for(j=0; j<cols; j++){
				printf("%d ", res[i][j]);
			}
			printf("\n");
		}
		*/
		///拷贝进程0的结果
		int ress[R1][C2];
		for(i=0; i<rows; i++){
			for(j=0; j<cols; j++){
			///	printf("%d %d\n", i, j);
				ress[i][j] = res[i][j];
			}
		}
		int cur_rows=0;	///最终结果矩阵当前行
		int cur_cols;
		///拷贝进程1 to size-1的结果
		int a, b, c_pro;
		for(a=0; a<m1_pros; a++){
			cur_cols = 0;
			for(b=0; b<m2_pros; b++){
				c_pro = a*m2_pros+b;///当前进程号
				if(c_pro == 0){
					cur_cols = cols;
					continue;
				}
				//printf("%d %d %d\n", c_pro, cur_rows, cur_cols);
				
				///printf("---------------%d\n", c_pro);
				MPI_Recv(result, arr_dis_rows[c_pro]*arr_dis_cols[c_pro], MPI_INT, c_pro, 101, comm, &status);
				/*
				for(j=0; j<arr_dis_rows[c_pro]*arr_dis_cols[c_pro]; j++){
					printf("%d ", result[j]);
				}
				printf("\n");
				*/
				
				for(j=0; j<arr_dis_rows[c_pro]; j++){
					for(k=0; k<arr_dis_cols[c_pro]; k++){
						ress[cur_rows+j][cur_cols+k] = result[j*arr_dis_cols[c_pro]+k];
					///	printf("%d %d\n", cur_rows+j, cur_cols+k);
					}
				}
				cur_cols += arr_dis_cols[c_pro];
			}
			cur_rows += arr_dis_rows[c_pro];
		}
		
		/*
		///屏幕打印结果
		for(i=0; i<R1; i++){
			for(j=0; j<C2; j++){
				printf("%d ", ress[i][j]);
			}
			printf("\n");
		}
		*/
		
		FILE *fpt;
		fpt = fopen("out2", "w");
		for(i=0; i<R1; i++){
			fprintf(fpt, "%d", ress[i][0]); 
			for(j=1; j<C2; j++){
				fprintf(fpt, " %d", ress[i][j]); 
			}
			fprintf(fpt,"\n"); 
		}
		fclose(fpt);
	}

    MPI_Finalize();
    return 0;
}