#include<stdio.h>
#include<time.h>
#define R1 1000
#define C1 1000
#define R2 1000
#define C2 1000
int main(){
	time_t s=0, e=0;
	s = clock();
	int matrix1[R1][C1];
	int matrix2[R2][C2];
	int matrix3[R1][C2];
	int i, j, k, count;
	count=1;
	for(i=0; i<R1; i++)
		for(j=0; j<C1; j++)
			matrix1[i][j] = count;
	count=1;
	for(i=0; i<R2; i++)
		for(j=0; j<C2; j++)
			matrix2[i][j] = count;
	for(i=0; i<R1; i++){
		for(j=0; j<C2; j++){
			matrix3[i][j]=0;
			for(k=0; k<R2; k++){
				matrix3[i][j] += matrix1[i][k]*matrix2[k][j];
			}
		}
	}
	///输出结果到文件
	FILE *fpt;
	fpt = fopen("out", "w");
	for(i=0; i<R1; i++){
		fprintf(fpt, "%d", matrix3[i][0]); 
		for(j=1; j<C2; j++)
			fprintf(fpt, " %d", matrix3[i][j]); 
		fprintf(fpt,"\n"); 
	}
	fclose(fpt);
	e = clock();
	printf("time: %ld ms\n", (e-s)/1000);
	return 0;
}