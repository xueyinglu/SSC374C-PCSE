#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>
void initialize(float **x,int n){
    srand(time(NULL));
    for (int i= 0; i<n; i++)
    for (int j= 0; j<n; j++){
        x[i][j]=(float)rand()/(float)RAND_MAX;
        //printf("x[i][j]= %f\n",x[i][j]);
    }
}

void smooth(float **x,float **y,int n,float a,float b,float c){
    int i;
#pragma omp parallel 
{
#pragma omp for schedule(dynamic,100000)
    for (i= 1; i<n-1; i++){
        int j;
        //printf("This is thread %d, i=%d\n", omp_get_thread_num(), i);
        for (j= 1; j<n-1; j++){
            y[i][j] = a*(x[i-1][j-1] + x[i-1][j+1] + x[i+1][j-1] + x[i+1][j+1]) 
                        + b*(x[i-1][j+0] + x[i+1][j+0] + x[i+0][j-1] + x[i+0][j+1]) 
                        + c* x[i+0][j+0];
        }
    }
}
}

void count(float **y,int n,float t,int *ct){
int localct=*ct;
int i,j;
#pragma omp parallel
{
#pragma omp for reduction(+:localct) private(i,j)
    for (i= 1; i<n-1; i++){
    for (j= 0; j<n-1; j++){
        if (y[i][j]<t) {localct+=1;}
    }
}
}
*ct=localct;
}

int main(){
#pragma omp parallel
{
printf("This is thread %d\n", omp_get_thread_num());
}
    long n = 98306;

//    int n=20;
    float a = 0.05;
    float b = 0.1;
    float c = 0.4;
    float t= 0.1;
    int ctx = 0;
    int cty = 0;
    float a1=clock();
    float **x = malloc(sizeof *x * n);
    if (x)
    {
        for (int i = 0; i < n; i++)
        {
            x[i] = malloc(sizeof *x[i] * n);
        }
    }
    float a2=clock();
    float t1= (a2 - a1) / (float)CLOCKS_PER_SEC;

    float a3=clock();
    float **y = malloc(sizeof *y * n);
    if (y)
    {
        for (int i = 0; i < n; i++)
        {
            y[i] = malloc(sizeof *y[i] * n);
        }
    }
    float a4=clock();
    float t2= (a4 - a3)/(float)CLOCKS_PER_SEC;
    double i1=omp_get_wtime();
    initialize(x,n);
    double i2=omp_get_wtime();
    double t3= (i2 - i1) ;
    double s1=omp_get_wtime();
    smooth(x,y,n,a,b,c);
    double s2=omp_get_wtime();
    double t4= (s2 - s1) ;
    double c1=omp_get_wtime();
    count(x,n,t,&ctx);
    double c2=omp_get_wtime();
    double t5= (c2 - c1);
    double c3=omp_get_wtime();
    count(y,n,t,&cty);
    double c4=omp_get_wtime();
    double t6= (c4 - c3);


    printf("Summary:\n");
    printf("Number of elements in a row/column: %d\n",n);
    printf("Number of inner elements in a row/column: %d\n",n-2);
    printf("Total number of elements: %d\n",n*n);
    printf("Total number of inner elements: %d\n",(n-2)*(n-2));
    printf("Memory (GB) used per array: %f\n", n*n*4*1e-9);
    printf("Threshold: %f\n", t);
    printf("Smoothing constants (a, b, c): %f %f %f \n",a,b,c);
    printf("Number of elements below threshold (X): %d\n",ctx);
    printf("Fraction of elements below threshold: %f\n", (float)ctx/((float)n*n));
    printf("Number of elements below threshold (Y): %d\n",cty);
    printf("Fraction of elements below threshold: %f\n", (float)cty/((float)n*n));
    printf("Action    Time     Time resolution = %f\n",1.0/(float)CLOCKS_PER_SEC);
    printf("CPU: Alloc-X: %f\n",t1);    
    printf("CPU: Alloc-Y: %f\n",t2);    
    printf("CPU: Init-X: %f\n",t3);    
    printf("CPU: Smooth: %f\n",t4);    
    printf("CPU: Count-X: %f\n",t5);    
    printf("CPU: Count-Y: %f\n",t6);    

    return 0;
};



