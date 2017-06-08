#include<stdio.h>
#include<math.h>
#include<time.h>
#define PI 3.14159265358979
int main(int argc, char** argv){
	time_t s=0, e=0;
	s = clock();
	double a=0, b=100*PI, sum=0, width=0.000001;	
	for(; a<=b; a+=width){
		sum += fabs(cos(a)*width);
	}
	printf("cos(x) integarte is %lf\n", sum);	
	e = clock();
	printf("time: %ldms\n", (e-s)/1000);
	return 0;
}

	
