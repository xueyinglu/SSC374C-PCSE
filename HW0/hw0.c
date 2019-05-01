#include <stdlib.h>
#include <time.h>
#include <stdio.h>

void initialize(float **x,int n){
    srand(time(NULL));
    for (int i= 0; i<n; i++)
    for (int j= 0; j<n; j++){
        x[i][j]=(float)rand()/(float)RAND_MAX;
        //printf("x[i][j]= %f\n",x[i][j]);
    }
}

void smooth(float **x,float **y,int n,float a,float b,float c){
    for (int i= 1; i<n-1; i++)
    for (int j= 1; j<n-1; j++){
    y[i][j] = a*(x[i-1][j-1] + x[i-1][j+1] + x[i+1][j-1] + x[i+1][j+1]) + b*(x[i-1][j+0] + x[i+1][j+0] + x[i+0][j-1] + x[i+0][j+1]) + c* x[i+0][j+0];
    }
    

}

void count(float **y,int n,float t,int *ct){
    for (int i= 1; i<n-1; i++)
    for (int j= 0; j<n-1; j++){
        if (y[i][j]<t) {*ct+=1;}
    }
}

int main(){
    unsigned long int n = 98306;
   // int n=10;
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
    float t2= (a4 - a3) / (float)CLOCKS_PER_SEC;
    float i1=clock();
    initialize(x,n);
    float i2=clock();
    float t3= (i2 - i1) / (float)CLOCKS_PER_SEC;
    float s1=clock();
    smooth(x,y,n,a,b,c);
    float s2=clock();
    float t4= (i2 - i1) / (float)CLOCKS_PER_SEC;
    float c1=clock();
    count(x,n,t,&ctx);
    float c2=clock();
    float t5= (c2 - c1) / (float)CLOCKS_PER_SEC;
    float c3=clock();
    count(y,n,t,&cty);
    float c4=clock();
    float t6= (c4 - c3) / (float)CLOCKS_PER_SEC;


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



